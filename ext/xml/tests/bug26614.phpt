--TEST--
Bug #26614 (CDATA sections skipped on line count)
--FILE--
<?php
$xml ='<?xml version="1.0"?>
<data>
<![CDATA[
multi
line 
CDATA
block
]]>
</data>';

function startElement($parser, $name, $attrs) {
    echo "<$name> at line ".xml_get_current_line_number($parser)."\n";
}
function endElement($parser, $name) {
    echo "</$name> at line ".xml_get_current_line_number($parser)."\n";
}

$xml_parser = xml_parser_create();
xml_set_element_handler($xml_parser, "startElement", "endElement");
xml_parse($xml_parser, file_get_contents("test.xml", true));
xml_parser_free($xml_parser);
?>
--EXPECT--
<DATA> at line 2
</DATA> at line 4
