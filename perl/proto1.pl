#!/usr/bin/perl
use strict;
################################################################################
## REQUIREMENTS FOR A WORKER
##
## package $module; {
##      ...
##    sub init  { ... }
##    sub $routine { ... }
## }
##
################################################################################
use lib qw(.);
#
# Dependencies:
#
use AnyEvent;
use AnyEvent::Fork;
use AnyEvent::Fork::Pool;
use AnyEvent::Log;
use threads qw[ yield ];
use threads::shared;
#
# Log settings
#
sub custom_formatter($$$$) {
   my $ts = AnyEvent::Log::format_time $_[0];
   my $ct = " ";
   my @res;
   for (split /\n/, sprintf "\e[0;37m%-5s \e[33;1m%s: \e[0;32m%s\e[0m",
            $AnyEvent::Log::LEVEL2STR[$_[2]], $_[1][0], $_[3]) {
      push @res, "$ts$ct$_\n";
      $ct = " + ";
   }
   join "", @res
}
sub make_passed($) {
    my($str) = @_;
    return "\e[0;32;1m$str\e[0m";
}

sub make_failed($) {
    my($str) = @_;
    return "\e[0;31;1m$str\e[0m";
}

sub AnyEvent::Log::format_time($) { 
    my $i = int $_[0];
    my $f = sprintf "%06d", 1e6 * ($_[0] - $i);
    my($now_int, $now_str1, $now_str2) = ($i, split /\x01/,
        POSIX::strftime "\e[0;36m%Y-%m-%d %H:%M:%S.\x01 %z\e[0m", localtime $i) if my $now_int != $i;
   "$now_str1$f$now_str2";
}
$AnyEvent::Log::FILTER->level ("info");
$AnyEvent::Log::LOG->fmt_cb (sub {
   my ($time, $ctx, $lvl, $msg) = @_;
   custom_formatter $time, $ctx, $lvl, $msg
});

my $debug_log = AnyEvent::Log::logger "debug" => \my $debug;
my $info_log = AnyEvent::Log::logger "info" => \my $info;

#
# Start pool
#
my $cpus = AnyEvent::Fork::Pool::ncpu 1;
my $module = "Worker";
my $routine = "start";

$info_log->("start the worker pool") if $info;
my $pool = AnyEvent::Fork
  ->new
  ->require ($module)
  ->AnyEvent::Fork::Pool::run(
      "${module}::$routine",         # Routine of a worker
      
      # pool management
      init  => "${module}::init",    # Initialization of a worker
      max   => $cpus,                # Number of acltive workers in the pool
      idle  => 0,                    # Number of idle workers in the pool
      load  => 1,                    # Size of the query of a worker
      start => 0.8,                  # timeout before starting a new process
      stop  => 10,                   # timeout before stopping an idle process
      on_destroy => (my $finisher = AE::cv), # called when pool is destroyed

      # AnyEvent::Fork::Pool management
      async      => 0,
      on_error   => sub { die "FATAL ERROR: $_[0]\n" },
      on_event   => sub { $debug_log->($_[0] => "requests handled: $_[0] \n") if $debug; },
  );
$info_log->("create task queue");
# TODO: 
my @queue = qw{passed failed};
my $queue_size : shared  = @queue;
my $finished_tasks : shared = 0;

$info_log->("start workers");
for my $task_args (@queue) {
  $pool->($task_args, sub {
      my ($test_case_name, $test_case_result, $test_case_additional) = @_;
      my $mask = "%s: %s%s\n";
      printf $mask, $test_case_name, 
             $test_case_result == 0 ? make_passed("PASSED") 
                                    : make_failed("FAILED"),
             ((length $test_case_additional > 0)
                                    ? ": ".$test_case_additional
                                    : $test_case_additional);
      $finished_tasks++;
  });
};

my $isReady : shared = 0; # 1 - all is done
my $isOk : shared  = 0;

my $idle_protector = AnyEvent->idle (cb => 
    sub { 
        if ($finished_tasks < $queue_size) {
            $isOk = 1;
            sleep 1;
            $isOk = 0;
        }
        else {
            $debug_log->("destoy the pool\n") if $debug;
            $isReady = 1;
            undef $pool;
        }
});

#
# Progress bar
#
async {
    local $| = 1;
    while ( !$isReady ) {
        do {
            select undef, undef, undef, 0.2;
            printf "\r waiting ... ($_) " if ($isOk);
          }
          for qw[ / - \ | * ];
    }
    printf "\n";
    $isReady = 0;
}->detach;

#
# Start loop
#
$finisher->recv;
################################################################################
