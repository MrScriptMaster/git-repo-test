#!/usr/bin/perl
use strict;

sub closure($$) {
	my $arg1 = shift;
	my $arg2 = shift;
	return sub ($) {
		my $arg3 = shift;
		print "$arg1 ","$arg2"," $arg3\n";
	}
}

my $clos = closure "one", "two";
$clos->("three");
