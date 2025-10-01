--TEST--
NAN coerced to other types
--FILE--
<?php

$nan = fdiv(0, 0);
var_dump($nan);

function implicit_to_bool(bool $v) {
    var_dump($v);
}
function implicit_to_string(string $v) {
    var_dump($v);
}

implicit_to_bool($nan);
implicit_to_string($nan);

var_dump((int) $nan);
var_dump((bool) $nan);
var_dump((string) $nan);
var_dump((array) $nan);
var_dump((object) $nan);

$types = [
    'null',
    'bool',
    'int',
    'string',
    'array',
    'object',
];

foreach ($types as $type) {
    $nan = fdiv(0, 0);
    settype($nan, $type);
    var_dump($nan);
}

?>
--EXPECTF--
float(NAN)

Warning: unexpected NAN value was coerced to bool in %s on line %d
bool(true)

Warning: unexpected NAN value was coerced to string in %s on line %d
string(3) "NAN"

Warning: The float NAN is not representable as an int, cast occurred in %s on line %d
int(0)

Warning: unexpected NAN value was coerced to bool in %s on line %d
bool(true)

Warning: unexpected NAN value was coerced to string in %s on line %d
string(3) "NAN"

Warning: unexpected NAN value was coerced to array in %s on line %d
array(1) {
  [0]=>
  float(NAN)
}

Warning: unexpected NAN value was coerced to object in %s on line %d
object(stdClass)#%d (1) {
  ["scalar"]=>
  float(NAN)
}

Warning: unexpected NAN value was coerced to null in %s on line %d
NULL

Warning: unexpected NAN value was coerced to bool in %s on line %d
bool(true)

Warning: The float NAN is not representable as an int, cast occurred in %s on line %d
int(0)

Warning: unexpected NAN value was coerced to string in %s on line %d
string(3) "NAN"

Warning: unexpected NAN value was coerced to array in %s on line %d
array(1) {
  [0]=>
  float(NAN)
}

Warning: unexpected NAN value was coerced to object in %s on line %d
object(stdClass)#%d (1) {
  ["scalar"]=>
  float(NAN)
}
