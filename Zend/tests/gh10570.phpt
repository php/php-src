--TEST--
GH-10570 (Assertion `(key)->h != 0 && "Hash must be known"' failed.): constant variation
--FILE--
<?php
$a = new stdClass();
for ($i = 0; $i < 2; $i++) {
    $a->{90};
    $a->{0} = 0;
}
?>
--EXPECTF--
Warning: Undefined property: stdClass::$90 in %s on line %d

Warning: Undefined property: stdClass::$90 in %s on line %d
