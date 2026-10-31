--TEST--
Bug #81481 (xml_get_current_byte_index limited to 32-bit numbers on 64-bit builds)
--CREDITS--
dev at b65sol dot com
--EXTENSIONS--
xml
--INI--
memory_limit=-1
--SKIPIF--
<?php
skipif(!getenv('RUN_RESOURCE_HEAVY_TESTS'), 'resource-heavy test');
require __DIR__ . '/libxml_expat_skipif.inc';
skipif(xml_uses_expat(), 'test is for libxml');
skipif(getenv('SKIP_SLOW_TESTS'), 'slow test');
skipif(PHP_INT_SIZE != 8, '64-bit only');
?>
--FILE--
<?php
$parser = xml_parser_create('UTF-8');
xml_set_element_handler( $parser, 'startelement', null );

$emptylong = str_repeat(' ', 1024*1024);
xml_parse($parser, '<root><i></i><b/><ext>Hello</ext>', false);
for($i = 0; $i < 2200; $i++) {
    xml_parse($parser, $emptylong, false);
}
xml_parse($parser, '<ext></ext><ext></ext></root>', false);

function startelement($parser, $name, $attribute) {
    if ( $name == 'EXT' ) { echo "Byte Index:", xml_get_current_byte_index($parser), "\n"; }
}
?>
--EXPECT--
Byte Index:21
Byte Index:2306867237
Byte Index:2306867248
