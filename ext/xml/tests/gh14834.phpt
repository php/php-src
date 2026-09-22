--TEST--
GH-14834 (Error installing PHP when --with-pear is used)
--EXTENSIONS--
xml
--SKIPIF--
<?php
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(want_expat: false);
?>
--FILE--
<?php
$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
    <!ENTITY foo "ent">
]>
<root>
  <element hint="hello&apos;world">&foo;<![CDATA[ &amp; ]]><?x &amp; ?></element>
</root>
XML;

$parser = xml_parser_create();
xml_set_character_data_handler($parser, function($_, $data) {
    var_dump($data);
});
xml_parse($parser, $xml, true);
?>
--EXPECT--
string(3) "
  "
string(3) "ent"
string(7) " &amp; "
string(1) "
"
