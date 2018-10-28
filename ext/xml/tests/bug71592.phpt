--TEST--
Bug #71592 (External entity processing never fails)
--SKIPIF--
<?php
if (!extension_loaded('xml')) die('skip xml extension not available');
?>
--FILE--
<?php
$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE p [
  <!ENTITY pic PUBLIC "image.gif" "http://example.org/image.gif">
]>
<root>
<p>&pic;</p>
<p></nop>
</root>
XML;

$parser = xml_parser_create_ns('UTF-8');
xml_set_external_entity_ref_handler($parser, function () {
    return false;
});
xml_parse($parser, $xml);
var_dump(xml_get_error_code($parser));
?>
===DONE===
--EXPECT--
int(21)
===DONE===
