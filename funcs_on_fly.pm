
package ButtonsMap; {

    use strict;

    my %codes = undef;
    my @devs = ("dev_1");
    my $device = "dev_1";
    
    sub new {
        my ($class) = @_;
        my $self = {};
        no strict 'refs';
        for my $k (keys %codes) {
            *{__PACKAGE__.'::'."$k"} = sub {
                my $self = shift;
                return %codes{$k};
            };
        }
        bless $self, $class;
        return $self;
    };
    
    sub getKeyStroke {
        my $akey = (join ' ', @_)[0];
        my @arr = split ' ', $akey;
        my %keys = @arr;
        my $stroke = "";
        my $first = 1;
        for my $k (keys %keys) {
            if ($first) {
                $stroke = %keys{$k};
                $first = 0;
            }
            else {
                $stroke .= " ".%keys{$k};
            }
        }
        return $stroke;
    }
    
    sub cleanup {
        %codes = undef;
        %codes = {};
    }

    sub print {
        print "Device $device\n";
        for my $k (sort keys %codes) {
            print "  $k:  $codes{$k}\n";
        }
        # foreach (keys %{*ButtonsMap::}) {
        #     print "  ".$_." => ".$ButtonsMap::{$_}."\n";
        # }
    }

    sub print_keys {
        print "Initialized keys:\n";
        my $c = 0;
        print "   ";
        for my $k (sort keys %codes) {
            print " $k" unless ($k =~ /HASH/);
            $c = ($c > 5 ? 0 : $c+1);
            if ($c == 0) {
                print "\n   ";
            }
        }
        print "\n";
    }

    sub init() {
        my($type) = @_;
        cleanup();
        $device = "dev_1" if (!length $type);
        for my $dev (@devs) {
            if ($device eq $dev) {
                init_dev_1();
                last;
            }
        } 
        #ButtonsMap::print();
    }

    sub init_dev_1 {
        my $rcodes = \%codes;
        $rcodes->{vol_up}         = 1;
        $rcodes->{vol_down}       = 2;
        $rcodes->{home}           = 3;
        $rcodes->{settings}       = 4;
        $rcodes->{speaker}        = 5;
        $rcodes->{headset}        = 6;
        $rcodes->{up}             = 7;
        $rcodes->{down}           = 8;
        $rcodes->{left}           = 9;
        $rcodes->{right}          = 10;
        $rcodes->{ok}             = 11;
        $rcodes->{soft_1}         = 12;
        $rcodes->{soft_2}         = 13;
        $rcodes->{soft_3}         = 14;
        $rcodes->{soft_4}         = 15;
    }
}

ButtonsMap::init();

1;
