#!/usr/bin/perl
use strict;
my @params = @ARGV;#qw( a b c );
my $outpath = "";
my $isRedirected = 0;
my $rc = 0;
sub _main(%)  {
    my $argv = $_[0];
    my $routine = $argv->{routine} || sub { };
    $argv->{result} = &$routine($argv->{params});
    return $argv->{result};
}
if (length $outpath)
{
    open (OUTPUT, '>', $outpath) and $isRedirected = 1;
    if ($isRedirected) {
        open STDOUT, '>&', OUTPUT;
        open STDERR, '>&', OUTPUT;
        select OUTPUT; $| = 1;
    }
}
$rc = _main({
    params => \@params,
    result => 0,
    routine => sub {
        my($argv) = @_;
        for my $elem (@$argv) {
            print "param: ".$elem."\n";
        }
        return 0;
    }
});
close OUTPUT if ($isRedirected);

$rc;
