--TEST--
Test key() function : basic functionality
--FILE--
<?php
/* Prototype  : mixed key(array $array_arg)
 * Description: Return the key of the element currently pointed to by the internal array pointer
 * Source code: ext/standard/array.c
 */

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
===DONE===
--EXPECTF--
*** Testing key() : basic functionality ***

-- Initial Position: --
int(0)

-- Next Position: --
int(99)

-- End Position: --
string(5) "three"

-- Past end of the array --
NULL
===DONE===
