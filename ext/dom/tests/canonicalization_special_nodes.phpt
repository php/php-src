--TEST--
Test: Canonicalization with special nodes
--EXTENSIONS--
dom
--FILE--
<?php
$xml = <<<EOXML
<?xml version="1.0"?>
<!DOCTYPE doc [
]>
<doc xmlns="">
    <![CDATA[bar]]>
    <!-- x -->
    <temp xmlns=""/>
    <?pi-no-data          ?>
</doc>
EOXML;

$dom = new DOMDocument();
$dom->loadXML($xml);
$doc = $dom->documentElement;
echo $doc->C14N(withComments: true);
echo $doc->C14N(withComments: false);

?>
--EXPECT--
<doc>
    bar
    <!-- x -->
    <temp></temp>
    <?pi-no-data?>
</doc><doc>
    bar
    
    <temp></temp>
    <?pi-no-data?>
</doc>
