--TEST--
xml_parse_into_struct() doesn't reach hard limit
--EXTENSIONS--
xml
--INI--
memory_limit=8M
--FILE--
<?php
// 2 MB == memory_limit / 4, as 512K empty elements
$data = "<r>" . str_repeat("<a/>", 512 * 1024) . "</r>";
$parser = xml_parser_create();
xml_parse_into_struct($parser, $data, $values, $index);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
