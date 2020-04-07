#!/usr/bin/perl

use strict;

my $Str="the number is ";
our $number = 10; # Глобально
{
    local $number = 5; # Пусть вас не смущает ключевое слово local: оно имеет немного другой смысл.
    print "With 'local' ".$Str.$number."\n"; # $number = 5 и до конца этого блока.
    print "In global area ".$Str.$main::number."\n"; # $number = 5. Глобальная переменная временно затенена.
}
print "After block ".$Str.$number." again\n"; # Снова $number = 10.
{
    my $number = 15; # Переменная существует только в этом блоке и извне к ней не обратиться.
    print "With 'my' ".$Str.$number."\n"; # $number = 15.
    print "But in global area ".$Str.$main::number."\n"; # $number = 10. К глобальным переменным мы обращаемся как обычно.
}
print "After block ".$Str.$number."\n"; # $number = 10. Глобальная переменная.

0
