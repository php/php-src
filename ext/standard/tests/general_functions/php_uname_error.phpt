--TEST--
Test php_uname() function -  error conditions - pass function incorrect arguments
--FILE--
<?php
/* Prototype: string php_uname  ([ string $mode  ] )
 * Description:  Returns information about the operating system PHP is running on
*/

echo "*** Testing php_uname() - error test\n";

echo "\n-- Testing php_uname() function with more than expected no. of arguments --\n";
var_dump( php_uname('a', true) );

echo "\n-- Testing php_uname() function with invalid mode --\n";
// am invalid mode shoudl result in same o/p as mode 'a'
var_dump( php_uname('z') == php_uname('z') ); 

class barClass {
}

$fp = fopen(__FILE__, "r");

echo "\n-- Testing php_uname() function with invalid argument types --\n";
var_dump(php_uname(array()));
var_dump(php_uname(array('color' => 'red', 'item' => 'pen')));
var_dump(php_uname(new barClass()));
var_dump(php_uname($fp));

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing php_uname() - error test

-- Testing php_uname() function with more than expected no. of arguments --

Warning: php_uname() expects at most 1 parameter, 2 given in %s on line %d
NULL

-- Testing php_uname() function with invalid mode --
bool(true)

-- Testing php_uname() function with invalid argument types --

Warning: php_uname() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: php_uname() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: php_uname() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: php_uname() expects parameter 1 to be string, resource given in %s on line %d
NULL
===DONE===