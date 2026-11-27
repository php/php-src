--TEST--
Test Uri\QueryParams serialization - success - empty list
--FILE--
<?php

$params1 = new Uri\QueryParams();
$params2 = unserialize(serialize($params1));

var_dump($params2->toList());
var_dump($params2->count());
$params2 = $params2->append("x", "1");
var_dump($params2->toRfc3986String());

?>
--EXPECT--
array(0) {
}
int(0)
string(3) "x=1"
