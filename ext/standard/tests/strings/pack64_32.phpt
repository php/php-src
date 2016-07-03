--TEST--
64bit pack()/unpack() tests
--SKIPIF--
<?php
if (PHP_INT_SIZE > 4) {
	die("skip 32bit test only");
}
?>
--FILE--
<?php
var_dump(pack("Q", 0));
var_dump(pack("J", 0));
var_dump(pack("P", 0));
var_dump(pack("q", 0));

var_dump(unpack("Q", ''));
var_dump(unpack("J", ''));
var_dump(unpack("P", ''));
var_dump(unpack("q", ''));
?>
--EXPECTF--
Warning: pack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: pack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: pack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: pack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: unpack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: unpack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: unpack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)

Warning: unpack(): 64-bit format codes are not available for 32-bit versions of PHP in %s on line %d
bool(false)
