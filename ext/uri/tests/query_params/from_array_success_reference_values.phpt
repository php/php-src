--TEST--
Test Uri\QueryParams::fromArray() - success - later changes to referenced values do not affect parameters
--FILE--
<?php

$value = "before";
$array = ["key" => &$value];
$params1 = Uri\QueryParams::fromArray($array);
$value = "after";
$params2 = Uri\QueryParams::fromArray($array);

var_dump($params1->getFirst("key"));
var_dump($params2->getFirst("key"));

?>
--EXPECT--
string(6) "before"
string(5) "after"
