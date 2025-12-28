--TEST--
GH-17187 (unreachable program point in zend_hash)
--EXTENSIONS--
xml
--CREDITS--
chongwick
--FILE--
<?php
class ImmutableParser {
    private $parser;
    private $immutableData;
    private $arrayCopy;

    public function __construct() {
        $this->parser = xml_parser_create();
        xml_set_element_handler($this->parser, function ($parser, $name, $attrs) {
            echo "open\n";
            var_dump($name, $attrs);
            $this->arrayCopy = [$this]; // Create cycle intentionally
            $this->immutableData = $this->arrayCopy;
        }, function ($parser, $name) {
            echo "close\n";
            var_dump($name);
        });
    }

    public function parseXml($xml) {
        $this->immutableData = array();
        xml_parse_into_struct($this->parser, $xml, $this->immutableData, $this->immutableData);
        return $this->immutableData;
    }
}
$immutableParser = new ImmutableParser();
$xml = "<container><child/></container>";
$immutableData = $immutableParser->parseXml($xml);
var_dump($immutableData);
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
array(5) {
  [0]=>
  object(ImmutableParser)#1 (3) {
    ["parser":"ImmutableParser":private]=>
    object(XMLParser)#2 (0) {
    }
    ["immutableData":"ImmutableParser":private]=>
    *RECURSION*
    ["arrayCopy":"ImmutableParser":private]=>
    array(1) {
      [0]=>
      *RECURSION*
    }
  }
  ["CHILD"]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  array(3) {
    ["tag"]=>
    string(5) "CHILD"
    ["type"]=>
    string(8) "complete"
    ["level"]=>
    int(2)
  }
  ["CONTAINER"]=>
  array(1) {
    [0]=>
    int(2)
  }
  [2]=>
  array(3) {
    ["tag"]=>
    string(9) "CONTAINER"
    ["type"]=>
    string(5) "close"
    ["level"]=>
    int(1)
  }
}
