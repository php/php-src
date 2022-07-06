--TEST--
Test prev() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of prev()
 */

echo "*** Testing prev() : basic functionality ***\n";

$array = array('zero', 'one', 'two');
end($array);
echo key($array) . " => " . current($array) . "\n";
var_dump(prev($array));

echo key($array) . " => " . current($array) . "\n";
var_dump(prev($array));

echo key($array) . " => " . current($array) . "\n";
var_dump(prev($array));

echo "\n*** Testing an array with differing values/keys ***\n";
$array2 = array('one', 2 => "help", 3, false, 'stringkey2' => 'val2', 'stringkey1' => 'val1');
end($array2);
$length = count($array2);
for ($i = $length; $i > 0; $i--) {
    var_dump(prev($array2));
}

?>
--EXPECT--
*** Testing prev() : basic functionality ***
2 => two
string(3) "one"
1 => one
string(4) "zero"
0 => zero
bool(false)

*** Testing an array with differing values/keys ***
string(4) "val2"
bool(false)
int(3)
string(4) "help"
string(3) "one"
bool(false)
