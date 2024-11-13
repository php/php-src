--TEST--
Test unserialize(): error is indistinguishable from deserialized boolean
--FILE--
<?php
echo "*** Testing unserialize() error/boolean distinction ***\n";

$garbage = "obvious non-serialized data";
$serialized_false = serialize(false);

var_dump($serialized_false);

$deserialized_garbage = unserialize($garbage);
var_dump($deserialized_garbage);

$deserialized_false = unserialize($serialized_false);
var_dump($deserialized_false);

echo "unserialize error and deserialized false are identical? " . (bool) ($deserialized_false == $deserialized_garbage) . "\n";

// candidate safe idiom for determining whether data is serialized
function isSerialized($str) {
    return ($str == serialize(false) || @unserialize($str) !== false);
}

// Test unserialize error idiom
var_dump(isSerialized($garbage));
var_dump(isSerialized($serialized_false));

echo "Done";
?>
--EXPECTF--
*** Testing unserialize() error/boolean distinction ***
string(4) "b:0;"

Warning: unserialize(): Error at offset 0 of 27 bytes in %s on line %d
bool(false)
bool(false)
unserialize error and deserialized false are identical? 1
bool(false)
bool(true)
Done
