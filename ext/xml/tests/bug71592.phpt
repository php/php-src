--TEST--
Bug #71592 (External entity processing never fails)
--EXTENSIONS--
xml
--FILE--
<?php
// The tag mismatch at the end of the XML is on purpose, to make sure that the
// parser actually stops after the handler returns FALSE.
$xml = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
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
var_dump(xml_get_error_code($parser) === XML_ERROR_EXTERNAL_ENTITY_HANDLING);
?>
--EXPECT--
bool(true)
