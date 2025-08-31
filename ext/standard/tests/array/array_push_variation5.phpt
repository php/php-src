--TEST--
Test array_push() function : usage variations - position of internal array pointer
--FILE--
<?php
/*
 * Check the position of the internal array pointer after calling array_push()
 */

echo "*** Testing array_push() : usage variations ***\n";

$stack = array ('one' => 'un', 'two' => 'deux');
$var0 = 'zero';

echo "\n-- Call array_push() --\n";
var_dump($result = array_push($stack, $var0));

echo "\n-- Position of Internal Pointer in Original Array: --\n";
echo key($stack) . " => " . current ($stack) . "\n";

echo "Done";
?>
--EXPECT--
*** Testing array_push() : usage variations ***

-- Call array_push() --
int(3)

-- Position of Internal Pointer in Original Array: --
one => un
Done
