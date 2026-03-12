--TEST--
Bug #76874: xml_parser_free() should never leak memory
--EXTENSIONS--
xml
--FILE--
<?php

class c
{
    private $xml;
    private $test;

    public function test()
    {
        $this->xml = xml_parser_create();
        xml_set_character_data_handler($this->xml, array(&$this, 'handle_cdata'));
        xml_parser_free($this->xml);
    }

    public function handle_cdata(&$parser, $data)
    {
    }
}

$object = new c();
$object->test();

?>
===DONE===
--EXPECTF--
Deprecated: Function xml_parser_free() is deprecated since 8.5, as it has no effect since PHP 8.0 in %s on line %d
===DONE===
