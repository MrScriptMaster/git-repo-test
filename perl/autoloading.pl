#!/usr/bin/perl
use strict;

Autoload::test();
Autoload::bla_bla(1,2,3);
Autoload::papapa();

package Autoload;
use strict;

our $AUTOLOAD;

sub AUTOLOAD {
	print $AUTOLOAD, " called not defined function with params: ",
		join (', ', @_), "\n";
}

sub test {
	print "testing\n";
}

1;
