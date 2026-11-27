--TEST--
Test Uri\QueryParams::fromArray() - success - empty input
--FILE--
<?php

$params = Uri\QueryParams::fromArray([]);

var_dump($params->toList());
var_dump($params->toArray());

?>
--EXPECT--
array(0) {
}
array(0) {
}
