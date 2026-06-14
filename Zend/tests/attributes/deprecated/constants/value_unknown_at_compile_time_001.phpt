--TEST--
#[\Deprecated]: Constant with value unknown at compile time.
--FILE--
<?php

define('SUFFIX', random_int(1, 2) == 1 ? 'a' : 'b');

#[\Deprecated]
const CONSTANT = 'Prefix-' . SUFFIX;

$value = CONSTANT;
var_dump($value);
var_dump($value === 'Prefix-' . SUFFIX);

?>
--EXPECTF--
Deprecated: Constant CONSTANT is deprecated in %s on line %d
string(8) "Prefix-%c"
bool(true)
