--TEST--
Test each() function : usage variations - Internal array pointer
--FILE--
<?php
/* Prototype  : array each(array $arr)
 * Description: Return the currently pointed key..value pair in the passed array,
 * and advance the pointer to the next element 
 * Source code: Zend/zend_builtin_functions.c
 */

/*
 * Test the position of the internal array pointer after a call to each()
 */

echo "*** Testing each() : usage variations ***\n";

$arr = array('zero', 'one', 'two', 'abc', 'xyz');

echo "\n-- Current position: --\n";
echo key($arr) . " => " . current($arr) . "\n";

echo "\n-- Call to each(): --\n";
var_dump( each($arr) );

echo "\n-- New position: --\n";
echo key($arr) . " => " . current($arr) . "\n";

echo "Done";
?>

--EXPECT--
*** Testing each() : usage variations ***

-- Current position: --
0 => zero

-- Call to each(): --
array(4) {
  [1]=>
  unicode(4) "zero"
  [u"value"]=>
  unicode(4) "zero"
  [0]=>
  int(0)
  [u"key"]=>
  int(0)
}

-- New position: --
1 => one
Done
