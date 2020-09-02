--TEST--
Test preg_match() function : variation
--FILE--
<?php
/* Function is implemented in ext/pcre/php_pcre.c */

//test passing in the same variable where 1 is by value, the other is a different
//type and by reference so should be updated to the new type.
$string = "-1";
preg_match('/[\-\+]?[0-9\.]*/', $string, $string);
var_dump($string);
?>
--EXPECT--
array(1) {
  [0]=>
  string(2) "-1"
}
