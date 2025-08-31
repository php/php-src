--TEST--
Bug #65236 (heap corruption in xml parser)
--EXTENSIONS--
xml
--FILE--
<?php
xml_parse_into_struct(xml_parser_create_ns(), str_repeat("<blah>", 1000), $a);

echo "Done\n";
?>
--EXPECTF--
Warning: xml_parse_into_struct(): Maximum depth exceeded - Results truncated in %s on line %d
Done
