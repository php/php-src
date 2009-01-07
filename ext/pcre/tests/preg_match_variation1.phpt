--TEST--
Test preg_match() function : variation
--FILE--
<?php
/* 
 *  proto int preg_match(string pattern, string subject [, array subpatterns [, int flags [, int offset]]])
 * Function is implemented in ext/pcre/php_pcre.c
*/

//test passing in the same variable where 1 is by value, the other is a different
//type and by reference so should be updated to the new type.
$string = "-1";
preg_match('/[\-\+]?[0-9\.]*/', $string, $string);
var_dump($string);
?>
===Done===
--EXPECT--
array(1) {
  [0]=>
  string(2) "-1"
}
===Done===
