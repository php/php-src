--TEST--
Test Uri\QueryParams::toArray() - success - empty list
--FILE--
<?php

$params = new Uri\QueryParams();


var_dump($params->toArray());

?>
--EXPECT--
array(0) {
}
