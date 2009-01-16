--TEST--
Test count_chars() function : basic functionality 
--FILE--
<?php

/* Prototype  : mixed count_chars  ( string $string  [, int $mode  ] )
 * Description: Return information about characters used in a string
 * Source code: ext/standard/string.c
*/


echo "*** Testing count_chars() : basic functionality ***\n";

$string = "Return information about characters used in a string";

var_dump(count_chars($string));
var_dump(count_chars($string, 0));
var_dump(count_chars($string, 1));
var_dump(count_chars($string, 2));
var_dump(count_chars($string, 3));
var_dump(bin2hex(count_chars($string, 4)));


?>
===DONE===
--EXPECTF--
*** Testing count_chars() : basic functionality ***
array(18) {
  [u"R"]=>
  int(1)
  [u"e"]=>
  int(3)
  [u"t"]=>
  int(5)
  [u"u"]=>
  int(3)
  [u"r"]=>
  int(5)
  [u"n"]=>
  int(5)
  [u" "]=>
  int(7)
  [u"i"]=>
  int(4)
  [u"f"]=>
  int(1)
  [u"o"]=>
  int(3)
  [u"m"]=>
  int(1)
  [u"a"]=>
  int(5)
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(2)
  [u"h"]=>
  int(1)
  [u"s"]=>
  int(3)
  [u"d"]=>
  int(1)
  [u"g"]=>
  int(1)
}

Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d
bool(false)
array(18) {
  [u"R"]=>
  int(1)
  [u"e"]=>
  int(3)
  [u"t"]=>
  int(5)
  [u"u"]=>
  int(3)
  [u"r"]=>
  int(5)
  [u"n"]=>
  int(5)
  [u" "]=>
  int(7)
  [u"i"]=>
  int(4)
  [u"f"]=>
  int(1)
  [u"o"]=>
  int(3)
  [u"m"]=>
  int(1)
  [u"a"]=>
  int(5)
  [u"b"]=>
  int(1)
  [u"c"]=>
  int(2)
  [u"h"]=>
  int(1)
  [u"s"]=>
  int(3)
  [u"d"]=>
  int(1)
  [u"g"]=>
  int(1)
}

Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d
bool(false)

Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d
bool(false)

Warning: count_chars(): Only mode=1 is supported with Unicode strings in %s on line %d
unicode(0) ""
===DONE===
