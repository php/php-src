--TEST--
Bug #31828 (Crash with zend.ze1_compatibility_mode=On)
--INI--
zend.ze1_compatibility_mode=on
--FILE--
<?php
$o = new stdClass();
$o->id = 77;
$o->name = "Aerospace";
$a[] = $o;
$a = $a[0];
print_r($a);
?>
--EXPECT--
stdClass Object
(
    [id] => 77
    [name] => Aerospace
)
