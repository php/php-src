--TEST--
Test explode() function : usage variations - positive and negative limits
--FILE--
<?php

echo "*** Testing explode() function: positive and negative limits ***\n";
$str = 'one||two||three||four';

echo "\n-- positive limit --\n";
var_dump(explode('||', $str, 2));

echo "\n-- negative limit (since PHP 5.1) --\n";
var_dump(explode('||', $str, -1));

echo "\n-- negative limit (since PHP 5.1) with null string -- \n";
var_dump(explode('||', "", -1));
?>
--EXPECT--
*** Testing explode() function: positive and negative limits ***

-- positive limit --
array(2) {
  [0]=>
  string(3) "one"
  [1]=>
  string(16) "two||three||four"
}

-- negative limit (since PHP 5.1) --
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
}

-- negative limit (since PHP 5.1) with null string -- 
array(0) {
}
