--TEST--
Test key() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of key()
 */

echo "*** Testing key() : basic functionality ***\n";

$array = array ('zero', 99 => 'one', 'two', 'three' => 3);
echo "\n-- Initial Position: --\n";
var_dump(key($array));

echo "\n-- Next Position: --\n";
next($array);
var_dump(key($array));

echo "\n-- End Position: --\n";
end($array);
var_dump(key($array));

echo "\n-- Past end of the array --\n";
next($array);
var_dump(key($array));
?>
--EXPECT--
*** Testing key() : basic functionality ***

-- Initial Position: --
int(0)

-- Next Position: --
int(99)

-- End Position: --
string(5) "three"

-- Past end of the array --
NULL
