--TEST--
Test Uri\QueryParams::toArray() - success - the default nesting limit is inclusive
--FILE--
<?php

$name = "k" . str_repeat("[a]", 64);
$params = (new Uri\QueryParams())->append($name, "value");
$array = $params->toArray();
for ($depth = 0; is_array($array); $depth++) {
    $array = reset($array);
}
var_dump($depth, $array);

?>
--EXPECT--
int(65)
string(5) "value"
