--TEST--
Bug #73135 (xml_parse() segmentation fault)
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
Warning: xml_parse(): Unable to call handler ahihi() in %s%ebug73135.php on line %d

Warning: xml_parse(): Unable to call handler ahihi() in %s%ebug73135.php on line %d