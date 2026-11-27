--TEST--
xml_error_string() maps libxml error codes 1 to 5 to the matching messages
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: false);
?>
--FILE--
<?php
$parser = xml_parser_create();
xml_parse($parser, '', true);
$code = xml_get_error_code($parser);
var_dump($code, xml_error_string($code));

for ($code = 1; $code <= 5; $code++) {
    var_dump(xml_error_string($code));
}
?>
--EXPECT--
int(4)
string(14) "Empty document"
string(14) "Internal error"
string(9) "No memory"
string(22) "Invalid document start"
string(14) "Empty document"
string(20) "Invalid document end"
