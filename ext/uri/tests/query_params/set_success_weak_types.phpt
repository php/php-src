--TEST--
Test Uri\QueryParams::set() - success - weak boolean and Stringable conversion
--FILE--
<?php

class Value {
    public function __toString(): string {
        return "0012";
    }
}

$params1 = Uri\QueryParams::parseRfc3986("a=old&b=old&c=old");
$params2 = $params1->set("a", false)->set("b", true)->set("c", new Value());

var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());

?>
--EXPECT--
string(17) "a=old&b=old&c=old"
string(14) "a=0&b=1&c=0012"
