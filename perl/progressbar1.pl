#!/usr/bin/perl

use strict;
use warnings;

my $CMD = q{ls -l > /dev/null};

for ([], [q{}], [q{dummy}], [$CMD, 10], [$CMD], [$CMD, 50]) {
    eval {
        run_with_progress(@$_);
    };
    warn $@ if $@;
}

sub run_with_progress {
    print q{-} x 60, qq{\nStarting ...\n};

    my ($cmd, $skip) = @_;

    die q{Nothing to run!} unless $cmd;
    $skip ||= 25;   # Arbitrary

    my @sails = map { "\b$_" } qw{- \ | /};
    my $sail = 0;

    {
        local $| = 1;

        print q{Running:  };

        for (0 .. 1000) {
            system $cmd and die qq{'$cmd' failed! $? : $!};

            print $sails[$sail++ % @sails] if not $_ % $skip;
        }
    }

    print qq{\b*\nFinished.\n};
}
