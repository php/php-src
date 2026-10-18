--TEST--
xml_parse() doesn't reach hard limit on attributes
--EXTENSIONS--
xml
--INI--
memory_limit=8M
--FILE--
<?php
// ~1 MB of markup, well under memory_limit / 4, as 100K empty attributes
$data = "<r";
for ($i = 0; $i < 100000; $i++) {
    $data .= " a$i=\"\"";
}
$data .= "/>";

$parser = xml_parser_create();
xml_set_element_handler($parser, function () {}, function () {});
xml_parse($parser, $data, true);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
