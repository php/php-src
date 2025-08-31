--TEST--
Bug #73135 (xml_parse() segmentation fault)
--EXTENSIONS--
xml
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
function start_elem($parser, $xml) {
    xml_parse($parser, $xml);
}

function dummy() {}

$xml = <<<HERE
<a xmlns="ahihi">
    <bar foo="ahihi"/>
</a>
HERE;

    $parser = xml_parser_create_ns();
    xml_set_element_handler($parser, 'start_elem', 'dummy');
    xml_parse($parser, $xml);
?>
--EXPECTF--
Fatal error: Uncaught Error: Parser must not be called recursively in %s:%d
Stack trace:
%a
