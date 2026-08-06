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
--EXPECT--
Deprecated: define(): Argument #3 ($case_insensitive) is ignored and treated as false since declaration of case-insensitive constants is no longer supported, passing the argument explicitly is unnecessary in /home/girgias/Dev/php-src/Zend/tests/builtin_functions/define_arg_3.php on line 3

Warning: define(): Argument #3 ($case_insensitive) is ignored since declaration of case-insensitive constants is no longer supported, this will be an error in PHP 9.0 in /home/girgias/Dev/php-src/Zend/tests/builtin_functions/define_arg_3.php on line 3
Error: Undefined constant "MY_CONSTANT"

Deprecated: define(): Argument #3 ($case_insensitive) is ignored and treated as false since declaration of case-insensitive constants is no longer supported, passing the argument explicitly is unnecessary in /home/girgias/Dev/php-src/Zend/tests/builtin_functions/define_arg_3.php on line 11
int(5)
