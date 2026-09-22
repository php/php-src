--TEST--
define() with 3rd argument
--FILE--
<?php

define('my_Constant', 5, true);

try {
	var_dump(MY_CONSTANT);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

define('MY_CONSTANT', 5, false);
var_dump(MY_CONSTANT);

?>
--EXPECTF--
Deprecated: define(): Argument #3 ($case_insensitive) is ignored and treated as false since declaration of case-insensitive constants is no longer supported, passing the argument explicitly is unnecessary in %s on line %d

Warning: define(): Argument #3 ($case_insensitive) is ignored since declaration of case-insensitive constants is no longer supported, this will be an error in PHP 9.0 in %s on line %d
Error: Undefined constant "MY_CONSTANT"

Deprecated: define(): Argument #3 ($case_insensitive) is ignored and treated as false since declaration of case-insensitive constants is no longer supported, passing the argument explicitly is unnecessary in %s on line %d
int(5)
