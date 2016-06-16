--TEST--
Defining constant twice with two different forms
--FILE--
<?php

define('a', 2);
const a = 1;


if (defined('a')) {
	print a;
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Constant a already defined in %sconstants_008.php:4
Stack trace:
#0 {main}
  thrown in %sconstants_008.php on line 4
