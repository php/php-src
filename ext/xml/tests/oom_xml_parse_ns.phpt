--TEST--
xml_parse() doesn't reach hard limit on namespaced attributes
--EXTENSIONS--
xml
--SKIPIF--
<?php
if (!@xml_parser_create_ns()) die("skip xml_parser_create_ns() not supported");
?>
--INI--
memory_limit=8M
--FILE--
<?php
// 8 KB URI x 4000 attributes == ~32 MB of expanded names from ~50 KB of input
$uri = str_repeat("u", 8192);
$data = "<r xmlns:p=\"$uri\"";
for ($i = 0; $i < 4000; $i++) {
    $data .= " p:a$i=\"\"";
}
$data .= "/>";

$parser = xml_parser_create_ns();
xml_set_element_handler($parser, function () {}, function () {});
xml_parse($parser, $data, true);
echo "not reached\n";
?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
