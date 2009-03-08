--TEST--
Test explode() function : usage variations - positive and negative limits
--FILE--
<?php

/* Prototype  : array explode  ( string $delimiter  , string $string  [, int $limit  ] )
 * Description: Split a string by string.
 * Source code: ext/standard/string.c
*/

echo "*** Testing explode() function: positive and negative limits ***\n";
$str = 'one||two||three||four';

echo "\n-- positive limit --\n";
var_dump(explode('||', $str, 2));

echo "\n-- negative limit (since PHP 5.1) --\n";
var_dump(explode('||', $str, -1));

echo "\n-- negative limit (since PHP 5.1) with null string -- \n";
var_dump(explode('||', "", -1));
?>
===DONE===
--EXPECT--
*** Testing explode() function: positive and negative limits ***

-- positive limit --
array(2) {
  [0]=>
  unicode(3) "one"
  [1]=>
  unicode(16) "two||three||four"
}

-- negative limit (since PHP 5.1) --
array(4) {
  [0]=>
  unicode(3) "one"
  [1]=>
  unicode(3) "two"
  [2]=>
  unicode(5) "three"
  [3]=>
  unicode(4) "four"
}

-- negative limit (since PHP 5.1) with null string -- 
array(0) {
}
===DONE===