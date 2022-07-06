--TEST--
Bug #50576 (XML_OPTION_SKIP_TAGSTART option has no effect)
--SKIPIF--
<?php
require_once("skipif.inc");
?>
--FILE--
<?php

$XML = <<<XML
<?xml version="1.0"?>
<ns1:listOfAwards xmlns:ns1="http://www.fpdsng.com/FPDS">
<ns1:count>
<ns1:total>867</ns1:total>
</ns1:count>
</ns1:listOfAwards>
XML;

$xml_parser = xml_parser_create();
xml_parser_set_option($xml_parser, XML_OPTION_SKIP_TAGSTART, 4);
xml_parse_into_struct($xml_parser, $XML, $vals, $index);
echo 'Index array' . PHP_EOL;
print_r($index);
echo 'Vals array' . PHP_EOL;
print_r($vals);
xml_parser_free($xml_parser);

function startElement($parser, $name, $attribs) { echo $name . PHP_EOL; }
function endElement($parser, $name) { echo $name . PHP_EOL; }
$xml_parser = xml_parser_create();
xml_set_element_handler($xml_parser, 'startElement', 'endElement');
xml_parser_set_option($xml_parser, XML_OPTION_SKIP_TAGSTART, 4);
xml_parse($xml_parser, $XML);
xml_parser_free($xml_parser);

?>
--EXPECT--
Index array
Array
(
    [LISTOFAWARDS] => Array
        (
            [0] => 0
            [1] => 5
            [2] => 6
        )

    [COUNT] => Array
        (
            [0] => 1
            [1] => 3
            [2] => 4
        )

    [TOTAL] => Array
        (
            [0] => 2
        )

)
Vals array
Array
(
    [0] => Array
        (
            [tag] => LISTOFAWARDS
            [type] => open
            [level] => 1
            [attributes] => Array
                (
                    [XMLNS:NS1] => http://www.fpdsng.com/FPDS
                )

            [value] => 

        )

    [1] => Array
        (
            [tag] => COUNT
            [type] => open
            [level] => 2
            [value] => 

        )

    [2] => Array
        (
            [tag] => TOTAL
            [type] => complete
            [level] => 3
            [value] => 867
        )

    [3] => Array
        (
            [tag] => COUNT
            [value] => 

            [type] => cdata
            [level] => 2
        )

    [4] => Array
        (
            [tag] => COUNT
            [type] => close
            [level] => 2
        )

    [5] => Array
        (
            [tag] => LISTOFAWARDS
            [value] => 

            [type] => cdata
            [level] => 1
        )

    [6] => Array
        (
            [tag] => LISTOFAWARDS
            [type] => close
            [level] => 1
        )

)
LISTOFAWARDS
COUNT
TOTAL
TOTAL
COUNT
LISTOFAWARDS
