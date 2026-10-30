--TEST--
GH-17187 (unreachable program point in zend_hash)
--EXTENSIONS--
xml
--CREDITS--
chongwick
--FILE--
<?php
class ImmutableParser {
    public $parser;
    public $immutableData1;
    public $immutableData2;

    public function __construct() {
        $this->parser = xml_parser_create();
        xml_set_element_handler($this->parser, function ($parser, $name, $attrs) {
            echo "open\n";
            var_dump($name, $attrs);
            $this->immutableData1 = 0xdead;
            $this->immutableData2 = 0xbeef;
        }, function ($parser, $name) {
            echo "close\n";
            var_dump($name);
        });
    }

    public function parseXml($xml) {
        $this->immutableData1 = array();
        $this->immutableData2 = array();
        xml_parse_into_struct($this->parser, $xml, $this->immutableData1, $this->immutableData2);
    }
}
$immutableParser = new ImmutableParser();
$xml = "<container><child/></container>";
$immutableParser->parseXml($xml);
var_dump($immutableParser->immutableData1);
var_dump($immutableParser->immutableData2);
?>
--EXPECT--
open
string(9) "CONTAINER"
array(0) {
}
open
string(5) "CHILD"
array(0) {
}
close
string(5) "CHILD"
close
string(9) "CONTAINER"
int(57005)
int(48879)
