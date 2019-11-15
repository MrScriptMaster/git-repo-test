#!/usr/bin/perl

use strict;
use XML::LibXML;

my $testXml = XML::LibXML->load_xml(string => <<'EOT');
<?xml version="1.0" encoding="utf-8"?>
<main>
    <tag attr="line">
        data
    </tag>
</main>
EOT

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
            
        }
    }
    return ($pParameters->{OverallPass});   
}

################################################################################

print (my $Version_String = XML::LibXML::LIBXML_DOTTED_VERSION."\n");
print (my $Version_ID = XML::LibXML::LIBXML_VERSION."\n");
print (my $DLL_Version = XML::LibXML::LIBXML_RUNTIME_VERSION."\n");
