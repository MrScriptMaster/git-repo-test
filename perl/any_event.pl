#!/usr/bin/perl

use lib qw(.);

use strict;
use AnyEvent::Fork;
use AnyEvent::Fork::Pool;

my $cpus = AnyEvent::Fork::Pool::ncpu 1;
my $module = "Worker";
my $sub = "work";

my $pool = AnyEvent::Fork
  ->new
  ->require ($module)
  ->AnyEvent::Fork::Pool::run(
      "${module}::$sub",         # Модуль::Функция - рабочая функция воркера
      init => "${module}::init", # Модуль::init - функция инициализации воркера
      max  => $cpus,             # Количество воркеров в пуле
      idle => 0,                 # Количество воркеров при простое
      load => 1,                 # Размер очереди воркера
  );

for my $str (qw{q2 rtr4 ui3 asdg5}) {
  $pool->($str, sub {
      print "result: @_\n";
  });
};

AnyEvent->condvar->recv;

######
package Worker;

use strict;
use warnings;

my $file;

sub init {
  open $file, '>>', 'file.txt';

  my $q = select($file);
  $|=1;
  select($q);

  return;
}

sub work {
  my ($str) = @_;

  for (1..length($str)) {
      print $file "$$ $str\n";
      sleep 1;
  };

  return $str, length($str);
}

1;
