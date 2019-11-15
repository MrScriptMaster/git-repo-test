################################################################################
## v0.9
##
## Dependencies:
##
use XML::LibXML;
use lib::Phone::AudioParameters ();
##
################################################################################
package Common; {

use strict;
use warnings;

    # constructor
    sub new {
        my ($class) = @_;
        my $self = {
            mDevice => 'common',
            mQuery => "<Unit-Type><instance><Element><Param>\n",
        };
        bless $self, $class;
        return $self;
    }

    sub getName {
        my($self) = @_;
        return $self->{mDevice};
    }
    
    sub getAudioEnvSize() {
        return scalar @Phone::AudioParameters::AudioEnvParams;
    }
    
    sub processFile($) {
        # Args
        my $FilePath = shift;
        # Locals
        my $xml		 = ();
        # End Locals
        
        if (-e $FilePath)
        {
            $xml = __getXmlReader($FilePath);
            # TODO: Make parse
        }
        else
        {
            die "Fatal error: [$FilePath] does not exist!";
        }
        return 0;
    }
    
    sub __getXmlReader() {
        my $file = shift;
        return (XML::LibXML->load_xml(location => $file, no_blanks => 1));
    }



    sub _fileWalk($$$$$$@);
    sub _fileWalk($$$$$$@)
    {
        # Args
        my $pParameters     = shift;    # Contains Validate, Bandwidth, etc. 
        my $UNIT            = shift;    # Used to build up the query and result string. UT_[WBHF] 0 UE_FIR_C DSPA_ADDR_OFFS_FIR_ESCL_O
        my $UNITINSTANCE    = shift;    # Used to build up the query and result string. UT_WBHF [0] UE_FIR_C DSPA_ADDR_OFFS_FIR_ESCL_O
        my $ELEMENT			= shift;	# Used to build up the query and result string. UT_WBHF 0 UE_[FIR]_C DSPA_ADDR_OFFS_FIR_ESCL_O
	    my $ELEMENTINSTANCE	= shift;	# Used to build up the query and result string. UT_WBHF 0 UE_FIR_[C] DSPA_ADDR_OFFS_FIR_ESCL_O
        my $PARAMETER		= shift;	# Used to build up the query and result string. UT_WBHF 0 UE_FIR_C DSPA_ADDR_OFFS_FIR_[ESCL_O]
        my @nodes = @_;
        # Locals

        # End Locals

        #???????
        $pParameters->{OverallPass} = 1 if (!defined $pParameters->{OverallPass});

        for my $node (@nodes)
        {
            if ($node->nodeName eq "WBHF" || 
               ($node->nodeName eq "AFE" && $UNIT eq ""))
            {
                _fileWalk($pParameters,
                          $node->nodeName,
                          $node->getAttribute('inst'),
                          $ELEMENT,
                          $ELEMENTINSTANCE,
                          $PARAMETER,
                          $node->childNodes);
            }
            elsif ($node->nodeName eq 'Profile' && 
                   $node->getAttribute('name') eq $pParameters->{Profile})
            {
                _fileWalk($pParameters,
                          $UNIT,
                          $UNITINSTANCE,
                          $ELEMENT,
                          $ELEMENTINSTANCE,
                          $PARAMETER,
                          $node->childNodes);
            }
            elsif ($UNIT ne "" && 
                   $UNITINSTANCE ne "" && 
                   $ELEMENT eq "" && 
                   $node->nodeType == XML::LibXML::XML_ELEMENT_NODE && 
                   $node->hasAttributes)
            {
                _fileWalk($pParameters,
                          $UNIT,
                          $UNITINSTANCE,
                          $node->nodeName,
                          $node->getAttribute('inst'),
                          $PARAMETER,
                          $node->childNodes);
            }
            elsif ($UNIT ne "" && 
                   $UNITINSTANCE ne "" && 
                   $ELEMENT ne "" && 
                   $ELEMENTINSTANCE ne "" && 
                   $PARAMETER eq "" && 
                   $node->nodeName ne "#comment")
            {
                __QueryFileBuildAddLine($UNIT,
                                        $UNITINSTANCE,
                                        $ELEMENT,
                                        $ELEMENTINSTANCE,
                                        $node->nodeName,
                                        $node);
            }
        }
        return ($pParameters->{OverallPass});   
    }

    sub __QueryFileBuildAddLine($$$$$$) 
    {
        # Args
        my $UNIT 			= shift;
        my $UNIT_INSTANCE	= shift;
        my $ELEMENT			= shift;
        my $ELEMENT_INSTANCE= shift;
        my $PARAMETER		= shift;
        my $pElement		= shift;
        # Locals
        my $Line			= "";
        # End Locals
        if (($PARAMETER eq "DENS_NL_att_buf") || 
            ($PARAMETER eq "DENS_gamma_e_fact") || 
            ($PARAMETER eq "DENS_IMD_att_buf"))
        {
            __QueryFileBuildAddLineArray($UNIT,
                                         $UNIT_INSTANCE,
                                         $ELEMENT,
                                         $ELEMENT_INSTANCE,
                                         $PARAMETER,
                                         $pElement);
        }
        else
        {
            $Line = __QueryFileLineMake($UNIT,
                                        $UNIT_INSTANCE,
                                        $ELEMENT,
                                        $ELEMENT_INSTANCE,
                                        $PARAMETER)."\n";
        
            $Line = __appendQuery($Line);
        }
        return 0;
    }

    sub __QueryFileBuildAddLineArray($$$$$$)
    {
        # Args
        my $UNIT 			= shift;
        my $UNIT_INSTANCE	= shift;
        my $ELEMENT			= shift;
        my $ELEMENT_INSTANCE= shift;
        my $PARAMETER		= shift;
        my $pElement		= shift;
        # Locals
        my $Index			= 0;
        my $Size			= 0;

        $pElement 			= __cleanUp($pElement->to_literal());
        my @Array 			= split(/ |\n/,$pElement);
        
        $Size = @Array;
        $Size = $Size - 1; # Size should be  0 based index. 
        $Size = length($Size);
        $Size  = "%0$Size"."d";
        foreach (@Array)
        {
            __QueryFileBuildAddLine($UNIT,
                                    $UNIT_INSTANCE,
                                    $ELEMENT,
                                    $ELEMENT_INSTANCE,
                                    $PARAMETER."_".sprintf($Size, $Index),
                                    $pElement);
            $Index++;
        }
        return 0;
    }

    sub __cleanUp($)
    {
        # Args
        my $ExpectedValue	= shift;
        # End Args
        #Whitespace before
        $ExpectedValue =~ s/^\s*//g;  
        #Whitespace after
        $ExpectedValue =~ s/\s*$//g;
        #Trailing \n space
        $ExpectedValue =~ s/\n\s*/\n/g;
        #Trailing space \n
        $ExpectedValue =~ s/\s*\n/\n/g;
        #0x
        $ExpectedValue =~ s/0x//g;
        #\t
        $ExpectedValue =~ s/\t//g;
        #\s+ (remove any double spaces with single space)
        $ExpectedValue =~ s/\s+/ /g;
        return ($ExpectedValue);  
    }

    sub __appendQuery() {
        my($self,$Line) = @_;
        
        if ($self->{mQuery} !~ /$Line/)
        {
            $self->{mQuery} .= $Line;
            $Line =~ s/\n//g;
        }
        return $Line;
    }

    sub __QueryFileLineMake($$$$$)
    {
        # Args
        my $UNIT 			= shift;
        my $UNIT_INSTANCE	= shift;
        my $ELEMENT			= shift;
        my $ELEMENT_INSTANCE= shift;
        my $PARAMETER		= shift;
        # Locals
        my $Line			= "";
        # End Locals

        if ($UNIT eq "WBHF")
        {
            if ($ELEMENT_INSTANCE eq "A")
            {
                $ELEMENT_INSTANCE = "";
            }
            else
            {
                $ELEMENT_INSTANCE = "_$ELEMENT_INSTANCE";
            }
            $Line = "UT_$UNIT $UNIT_INSTANCE UE_".$ELEMENT.$ELEMENT_INSTANCE." DSPA_ADDR_OFFS_".$ELEMENT."_".$PARAMETER;
        }
        elsif ($UNIT eq "AFE")
        {
            $Line = "$UNIT $UNIT_INSTANCE UE_".$UNIT." DAIF_$ELEMENT$ELEMENT_INSTANCE";
        }
        else
        {
            die "Unexpected UNIT: [$UNIT]";
        }
        
        return $Line;
    }

    sub test() {
        print @_;
    }
}

package Child; {
    
    use base Common;
    use strict;
    use warnings;

    sub new {
        my($class) = @_;
        my $self = Common::new($class);
        $self->{mDevice} = "Child";
        return $self;
    }

    sub getName {
        my($self) = @_;
        return $self->{mDevice}."\n";
    }
}

1; # End
