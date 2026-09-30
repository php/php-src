--TEST--
Test Uri\QueryParams::append() - success - weak boolean and Stringable conversion
--FILE--
<?php

class Value {
    public function __toString(): string {
        return "0012";
    }
}

$value = new Value();
$reference = &$value;
$params = (new Uri\QueryParams())->append("a", false)->append("b", true)->append("c", $reference);

var_dump($value instanceof Value);
var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "0"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(1) "1"
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    string(4) "0012"
  }
}
string(14) "a=0&b=1&c=0012"
string(14) "a=0&b=1&c=0012"
string(14) "a=0&b=1&c=0012"
