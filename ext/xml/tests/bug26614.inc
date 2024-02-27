<?php
/*
this test has different output on libxml2 (even depending on the version) and Expat

further investigation has shown that not only line count
is skippet on CDATA sections but that libxml does also
show different column numbers and byte positions depending
on context and in opposition to what one would expect to
see and what good old Expat reported just fine ...
*/

$xmls = array();

// Case 1: CDATA Sections
$xmls["CDATA"] ='<?xml version="1.0" encoding="iso-8859-1" ?>
<data>
<![CDATA[
multi
line
CDATA
block
]]>
</data>';

// Case 2: replace some characters so that we get comments instead
$xmls["Comment"] ='<?xml version="1.0" encoding="iso-8859-1" ?>
<data>
<!-- ATA[
multi
line
CDATA
block
-->
</data>';

// Case 3: replace even more characters so that only textual data is left
$xmls["Text"] ='<?xml version="1.0" encoding="iso-8859-1" ?>
<data>
-!-- ATA[
multi
line
CDATA
block
---
</data>';

function startElement($parser, $name, $attrs) {
    printf("<$name> at line %d, col %d (byte %d)\n",
               xml_get_current_line_number($parser),
               xml_get_current_column_number($parser),
               xml_get_current_byte_index($parser));
}

function endElement($parser, $name) {
    printf("</$name> at line %d, col %d (byte %d)\n",
               xml_get_current_line_number($parser),
               xml_get_current_column_number($parser),
               xml_get_current_byte_index($parser));
}

function characterData($parser, $data) {
  // dummy
}

foreach ($xmls as $desc => $xml) {
  echo "$desc\n";
    $xml_parser = xml_parser_create();
    xml_set_element_handler($xml_parser, "startElement", "endElement");
    xml_set_character_data_handler($xml_parser, "characterData");
    if (!xml_parse($xml_parser, $xml, true))
        echo "Error: ".xml_error_string(xml_get_error_code($xml_parser))."\n";
    xml_parser_free($xml_parser);
}
