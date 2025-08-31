--TEST--
GH-12254: xml_parse_into_struct() memory leak when called twice
--EXTENSIONS--
xml
--FILE--
<?php

$parser = xml_parser_create();
xml_set_element_handler($parser, function ($parser, $name, $attrs) {
    echo "open\n";
    var_dump($name, $attrs);
    var_dump(xml_parse_into_struct($parser, "<container/>", $values, $tags));
}, function ($parser, $name) {
    echo "close\n";
    var_dump($name);
});
xml_parse_into_struct($parser, "<container/>", $values, $tags);
// Yes, this doesn't do anything but it at least shouldn't leak...
xml_parse_into_struct($parser, "<container/>", $values, $tags);

?>
--EXPECTF--
open
string(9) "CONTAINER"
array(0) {
}

Warning: xml_parse_into_struct(): Parser must not be called recursively in %s on line %d
bool(false)
close
string(9) "CONTAINER"
