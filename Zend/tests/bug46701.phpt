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
--EXPECT--
array(3) {
  [-866368000]=>
  int(1)
  [-835511808]=>
  int(2)
  [-835350528]=>
  int(3)
}
int(2)
array(1) {
  [-835350528]=>
  int(3)
}
