--TEST--
Test xml_set_processing_instruction_handler function : basic
--EXTENSIONS--
xml
--FILE--
<?php
class XML_Parser
{

    function PIHandler($parser, $target, $data)
    {
        echo "Target: " . $target. "\n";
        echo "Data: " . $data . "\n";
    }

    function parse($data)
    {
        $parser = xml_parser_create();
        xml_set_object($parser, $this);
        xml_set_processing_instruction_handler($parser, "PIHandler");
        xml_parse($parser, $data, true);
        xml_parser_free($parser);
    }


}

$xml = <<<HERE
<?xml version="1.0" encoding="ISO-8859-1"?>
<?xml-stylesheet href="default.xsl" type="text/xml"?>
HERE;

echo "Simple test of xml_set_processing_instruction_handler() function\n";
$p1 = new Xml_Parser();
$p1->parse($xml);
echo "Done\n";
?>
--EXPECTF--
Simple test of xml_set_processing_instruction_handler() function

Deprecated: Function xml_set_object() is deprecated since 8.4, provide a proper method callable to xml_set_*_handler() functions in %s on line %d

Deprecated: xml_set_processing_instruction_handler(): Passing non-callable strings is deprecated since 8.4 in %s on line %d
Target: xml-stylesheet
Data: href="default.xsl" type="text/xml"
Done
