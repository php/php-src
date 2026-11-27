--TEST--
Test Uri\QueryParams::deleteValue() - success - weak boolean and Stringable conversion
--FILE--
<?php

class Value {
    public function __toString(): string {
        return "0012";
    }
}

$params1 = Uri\QueryParams::parseRfc3986("a=0&a=1&a=0012&a=&a");
$params2 = $params1->deleteValue("a", false)->deleteValue("a", true)->deleteValue("a", new Value());

var_dump($params1->toRfc3986String());
var_dump($params2->getAll("a"));

?>
--EXPECT--
string(19) "a=0&a=1&a=0012&a=&a"
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  NULL
}
