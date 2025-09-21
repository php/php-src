--TEST--
Bug #46701 (Creating associative array with long values in the key fails on 32bit linux)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die('skip this test is for 32bit platforms only'); ?>
--FILE--
<?php

$test_array = array(
    0xcc5c4600 => 1,
    0xce331a00 => 2
);
$test_array[0xce359000] = 3;

var_dump($test_array);
var_dump($test_array[0xce331a00]);

class foo {
    public $x;

    public function __construct() {
        $this->x[0xce359000] = 3;
        var_dump($this->x);
    }
}

new foo;

?>
--EXPECTF--
Warning: The float 3428599296 is not representable as an int, cast occurred in %s on line %d

Warning: The float 3459455488 is not representable as an int, cast occurred in %s on line %d

Warning: The float 3459616768 is not representable as an int, cast occurred in %s on line %d
array(3) {
  [-866368000]=>
  int(1)
  [-835511808]=>
  int(2)
  [-835350528]=>
  int(3)
}

Warning: The float 3459455488 is not representable as an int, cast occurred in %s on line %d
int(2)

Warning: The float 3459616768 is not representable as an int, cast occurred in %s on line %d
array(1) {
  [-835350528]=>
  int(3)
}
