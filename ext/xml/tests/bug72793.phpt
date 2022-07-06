--TEST--
Bug #72793: xml_parser_free leaks mem when execute xml_set_object
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

class xml  {
    var $parser;

    function __construct()
    {
        $this->parser = xml_parser_create();
        xml_set_object($this->parser, $this);
    }

    function parse($data)
    {
        xml_parse($this->parser, $data);
    }

    function free(){
        xml_parser_free($this->parser);
    }
}

$xml_test = '<?xml version="1.0" encoding="utf-8"?><test></test>';
$xml_parser = new xml();
$xml_parser->parse($xml_test);
$xml_parser->free();

?>
===DONE===
--EXPECT--
===DONE===
