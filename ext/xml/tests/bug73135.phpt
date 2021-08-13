--TEST--
Bug #73135 (xml_parse() segmentation fault)
--SKIPIF--
<?php if (!extension_loaded('xml')) die('skip xml extension not loaded'); ?>
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
    function start_elem($parser, $xml) {
        xml_parse($parser, $xml);
    }

    $xml = <<<HERE
    <a xmlns="ahihi">
        <bar foo="ahihi"/>
    </a>
HERE;

    $parser = xml_parser_create_ns();
    xml_set_element_handler($parser, 'start_elem', 'ahihi');
    xml_parse($parser, $xml);
?>
--EXPECTF--
Fatal error: Uncaught Error: Parser must not be called recursively in %s:%d
Stack trace:
%a
