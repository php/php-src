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
Warning: Constant a already defined, this will be an error in PHP 9 in %s on line %d
2
