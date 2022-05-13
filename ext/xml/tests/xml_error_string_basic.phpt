--TEST--
xml_error_string() - Basic test on 5 error codes
--EXTENSIONS--
xml
--FILE--
<?php
$xmls = array(
    '<?xml version="1.0"?><element>',
    '<?xml>',
    '<?xml version="dummy">',
    '<?xml?>',
    '<?xml version="1.0"?><elem></element>',
);

foreach ($xmls as $xml) {
    $xml_parser = xml_parser_create();
    if (!xml_parse($xml_parser, $xml, true)) {
        var_dump(xml_get_error_code($xml_parser));
        var_dump(xml_error_string(xml_get_error_code($xml_parser)));
    }
    xml_parser_free($xml_parser);
}
?>
--EXPECT--
int(5)
string(20) "Invalid document end"
int(47)
string(35) "Processing Instruction not finished"
int(57)
string(28) "XML declaration not finished"
int(64)
string(17) "Reserved XML Name"
int(76)
string(14) "Mismatched tag"
