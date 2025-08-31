--TEST--
DomDocument::createProcessingInstruction() - basic test for DomDocument::createProcessingInstruction()
--CREDITS--
Muhammad Khalid Adnan
# TestFest 2008
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;

$node = $doc->createElement("para");
$newnode = $doc->appendChild($node);

$test_proc_inst0 =
    $doc->createProcessingInstruction( "blablabla" );
$node->appendChild($test_proc_inst0);

$test_proc_inst1 =
    $doc->createProcessingInstruction( "blablabla", "datadata" );
$node->appendChild($test_proc_inst1);

echo $doc->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<para><?blablabla?><?blablabla datadata?></para>
