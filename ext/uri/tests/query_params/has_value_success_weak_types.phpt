--TEST--
Test Uri\QueryParams::hasValue() - success - weak boolean and Stringable conversion
--FILE--
<?php

class Value {
    public function __toString(): string {
        return "0012";
    }
}

$params = Uri\QueryParams::parseRfc3986("a=0&a=1&b=0012&c=");

var_dump($params->hasValue("a", false));
var_dump($params->hasValue("a", true));
var_dump($params->hasValue("b", new Value()));
var_dump($params->hasValue("c", false));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
