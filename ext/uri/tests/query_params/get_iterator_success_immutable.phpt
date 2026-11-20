--TEST--
Test Uri\QueryParams::getIterator() - success - an iterator retains the original parameters
--FILE--
<?php

function createParams(&$iterator): Uri\QueryParams {
    $params = Uri\QueryParams::parseRfc3986("x=1&x=2");
    $iterator = $params->getIterator();
    return $params;
}

$params = createParams($iterator)->set("x", "3");

var_dump(iterator_to_array($iterator) === [["x", "1"], ["x", "2"]]);
var_dump($params->toRfc3986String());

?>
--EXPECT--
bool(true)
string(3) "x=3"
