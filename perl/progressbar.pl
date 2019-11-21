#!/usr/bin/perl
use strict;
use threads qw[ yield ];
use threads::shared;

# by chanio of perlmonks
# This alternative should print a line only if it succeeds 
# in what is doing as job. That way, there is a real 
# significance of the spinning wheel (sort of:). When the 
# job is not succeeding, the wheel should stay calm (panic!).


my $ready : shared = 0;
my $isOk : shared  = 0;

async {
    local $| = 1;
    while ( !$ready ) {
        do {
            select undef, undef, undef, 0.2;
            printf "\r ($_)" if ($isOk);
          }
          for qw[ / - \ | * ];
    }
    print "\rReady";
    $ready = 0;
  }
  ->detach;

# do your work here
for ( 1 .. 10 ) {

## Busy, busy, busy
    $isOk = 1;
    sleep 1;
    $isOk = 0;
}

$isOk = 0;

$ready = 1;
yield while $ready;
