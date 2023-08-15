--TEST--
SimpleXMLElement::asXML() with a fragment and a filename
--EXTENSIONS--
simplexml
--FILE--
<?php

$sxe = simplexml_load_string(<<<XML
<?xml version="1.0"?>
<container>
    <container2>
        <child id="foo">bar</child>
    </container2>
</container>
XML);
$sxe->container2->asXML(__DIR__."/SimpleXMLElement_asXML_fragment_filename_output.tmp");

// Note: the strange indent is correct: the indent text node preceding container2 is not emitted.
echo file_get_contents(__DIR__."/SimpleXMLElement_asXML_fragment_filename_output.tmp");

?>
--CLEAN--
<?php
@unlink(__DIR__."/SimpleXMLElement_asXML_fragment_filename_output.tmp");
?>
--EXPECT--
<container2>
        <child id="foo">bar</child>
    </container2>
