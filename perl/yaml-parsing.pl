#!/usr/bin/perl
use strict;

use YAML qw();
use Data::Dumper;

my ($hashref, $arrayref, $string) = YAML::Load(<<'...');
---
defines:
    - &echo echo
test_name: example 1
device:
  IPv4: 192.168.0.1
  extension: 123456
plan:
  - step: shell
    cmd: *echo
    params: "Hello, World"
  - step: shell
    cmd: *echo
    params: "Goodbye, World"
  - step: timeout
    time_s: 1
...

print YAML::Dump($string, $arrayref, $hashref);
print "-----------------------------------------------\n";
print Dumper($string, $arrayref, $hashref);
print "-----------------------------------------------\n";

my %test_info = {
    name => "",
    ip4_address => "",
    extension => "",
    plan => (),
};

sub validate($) {
    my($ref_struct) = @_;

    while (my ($key, $value) = each(%$ref_struct)) {
        if ($key eq "plan") {
            for my $entry (@$value) {
                print "New entry ------\n";
                if ($entry->{step} eq "shell") {
                    printf " type: %s\n cmd: %s\n params: %s\n", 
                        $entry->{step},
                        $entry->{cmd},
                        $entry->{params};
                } 
                elsif ($entry->{step} eq "timeout") {
                    printf " type: %s\n time_s: %s\n", 
                        $entry->{step},
                        $entry->{time_s};
                }
                else {
                    print "undefined entry\n";
                }
            }
        }
        else {
            print "keyname = $key; value = $value"."\n";
        }
    }    
}

while (my ($key, $value) = each(%$hashref)) {
    if ($key eq "test_name") {
        $test_info{name} = $value;
    } elsif ($key eq "device") {
        while (my ($key_1, $value_1) = each(%$value)) {
            if ($key_1 eq "IPv4") {
                $test_info{ip4_address} = $value_1;
            } elsif ($key_1 eq "extension") {
                $test_info{extension} = $value_1;
            }
        }
    } elsif ($key eq "plan") {
        $test_info{plan} = $value;
    }
}
$hashref = undef;

validate(\%test_info);
