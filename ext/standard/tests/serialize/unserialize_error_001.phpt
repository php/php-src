--TEST--
Test unserialize() with non-bool/array allowed_classes
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

var_dump(unserialize($s, ["allowed_classes" => null]));
var_dump(unserialize($s, ["allowed_classes" => 0]));
var_dump(unserialize($s, ["allowed_classes" => 1]));
--EXPECTF--
Warning: unserialize(): allowed_classes option should be array or boolean in %s on line %d
bool(false)

Warning: unserialize(): allowed_classes option should be array or boolean in %s on line %d
bool(false)

Warning: unserialize(): allowed_classes option should be array or boolean in %s on line %d
bool(false)
