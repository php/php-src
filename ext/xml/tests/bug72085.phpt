--TEST--
Bug #72085 (SEGV on unknown address zif_xml_parse)
--EXTENSIONS--
xml
--FILE--
<?php
$var1 = xml_parser_create_ns();
xml_set_element_handler($var1, new Exception(""), 4096);
try {
    xml_parse($var1,  str_repeat("<a>", 10));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Invalid callback Exception::__invoke, no array or string given
