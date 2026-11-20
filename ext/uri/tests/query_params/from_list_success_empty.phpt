--TEST--
Test Uri\QueryParams::fromList() - success - empty input
--FILE--
<?php

$params = Uri\QueryParams::fromList([]);

var_dump($params->toList());
var_dump($params->count());

?>
--EXPECT--
array(0) {
}
int(0)
