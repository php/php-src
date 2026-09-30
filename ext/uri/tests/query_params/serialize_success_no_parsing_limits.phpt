--TEST--
Test Uri\QueryParams serialization - success - the parsing parameter count limit does not apply
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986(
    str_repeat("a=x&", 1000) . "a=x",
    maxParamCount: 0,
);
$params2 = unserialize(serialize($params1));

var_dump($params2->toList() === $params1->toList());
var_dump($params2->count());

?>
--EXPECT--
bool(true)
int(1001)
