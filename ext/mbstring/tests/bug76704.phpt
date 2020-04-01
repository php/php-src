--TEST--
Bug #76704 (mb_detect_order return value varies based on argument type)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php
try {
    var_dump(mb_detect_order('Foo, UTF-8'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(mb_detect_order(['Foo', 'UTF-8']))
?>
--EXPECTF--
mb_detect_order(): Argument #1 ($encoding) must be a list of valid encoding, "Foo" given

Warning: mb_detect_order(): Unknown encoding "Foo" in %s on line %d
bool(false)
