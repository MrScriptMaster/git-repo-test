#!/usr/bin/perl
use strict;
use warnings;

$\ = "\n";
my $person = Person->new(
    name    =>  'Smith Smithson',
    age     =>  '25',
    other   =>  'perl programmer',
);

print "Name: ", $person->get_name();
print "Age: ", $person->get_age();
print "Other: ", $person->get_other();

package Person;
use strict;
use warnings;

sub new {
    my ($class, %params) = @_;

    my $self = {};

    no strict 'refs';
    for my $key (keys %params) {
        # __PACKAGE__ равен текущему модулю, это встроенная
        # волшебная строка
        # следующая строка превращается в, например:
        # Person::get_name = sub {...};
        *{__PACKAGE__ . '::' . "get_$key"} = sub {
            my $self = shift;
            return $self->{$key};
        };
        $self->{$key} = $params{$key};
    }

    bless $self, $class;
    return $self;
}

1;
