--TEST--
Test Uri\QueryParams::fromArray() - success - the toArray nesting limit does not restrict construction
--FILE--
<?php

$array = "value";
for ($depth = 0; $depth < 66; $depth++) {
    $array = ["a" => $array];
}
$params = Uri\QueryParams::fromArray($array);

var_dump($params->count());
var_dump($params->getFirst("a" . str_repeat("[a]", 65)));
var_dump($params->toArray(65) === $array);

?>
--EXPECT--
int(1)
string(5) "value"
bool(true)
