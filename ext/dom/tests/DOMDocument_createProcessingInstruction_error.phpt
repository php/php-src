--TEST--
DomDocument::createProcessingInstruction() - error test for DomDocument::createProcessingInstruction()
--CREDITS--
Muhammad Khalid Adnan
# TestFest 2008
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;

$node = $doc->createElement("para");
$newnode = $doc->appendChild($node);

try {
    $test_proc_inst =
        $doc->createProcessingInstruction( "bla bla bla" );
    $node->appendChild($test_proc_inst);

    echo $doc->saveXML();
}
catch (DOMException $e)
{
    echo 'Test failed!', PHP_EOL;
}

?>
--EXPECT--
Test failed!

