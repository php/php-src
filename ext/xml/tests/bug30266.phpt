--TEST--
Bug #30266 (Invalid opcode 137/1/8)
--EXTENSIONS--
xml
--FILE--
<?php
/*

Currently (Feb 10, 2005) CVS HEAD fails with the following message:

Fatal error: Invalid opcode 137/1/8. in /home/hartmut/projects/php/dev/head/ext/xml/tests/bug30266.php on line 22

*/
class XML_Parser
{
    public $dummy = "a";

    function parse($data)
    {
        $parser = xml_parser_create();

        xml_set_object($parser, $this);

        xml_set_element_handler($parser, 'startHandler', 'endHandler');

        xml_parse($parser, $data, true);

        xml_parser_free($parser);
    }

    function startHandler($XmlParser, $tag, $attr)
    {
            $this->dummy = "b";
            throw new Exception("ex");
    }

    function endHandler($XmlParser, $tag)
    {
    }
}

$p1 = new Xml_Parser();
try {
    $p1->parse('<tag1><tag2></tag2></tag1>');
    echo "Exception swallowed\n";
} catch (Exception $e) {
    echo "OK\n";
}
?>
--EXPECTF--
Deprecated: Function xml_set_object() is deprecated since 8.4, provide a proper method callable to xml_set_*_handler() functions in %s on line %d

Deprecated: xml_set_element_handler(): Passing non-callable strings is deprecated since 8.4 in %s on line %d
OK
