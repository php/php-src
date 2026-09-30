--TEST--
Test Uri\QueryParams cloning - success - empty instance
--FILE--
<?php

$params1 = new Uri\QueryParams();
$params2 = clone $params1;
var_dump($params1 === $params2);

unset($params1);
$params2 = $params2->append("x", "1");
var_dump($params2->toRfc3986String());
var_dump($params2->count());

?>
--EXPECT--
bool(false)
string(3) "x=1"
int(1)
