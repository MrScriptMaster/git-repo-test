#!/usr/bin/perl
use strict;
use Data::Dumper;

sub wntarray {
	my @arr = (1,2,3);
	if (wantarray) {
		print "return array\n";
		return @arr;
	} else {
		print "return reference to array\n";
		return \@arr;
	}
}

my @result = wntarray();
my $result = wntarray();

print Dumper @result;
print Dumper $result;
