--TEST--
gmp_rootrem() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_rootrem(1000, 3));
var_dump(gmp_rootrem(100, 3));
var_dump(gmp_rootrem(-100, 3));

var_dump(gmp_rootrem(1000, 4));
var_dump(gmp_rootrem(100, 4));
try {
    var_dump(gmp_rootrem(-100, 4));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(gmp_rootrem(0, 3));

try {
    var_dump(gmp_rootrem(100, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_rootrem(100, -3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "10"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(1) "0"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(1) "4"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "36"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "-4"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(3) "-36"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(1) "5"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(3) "375"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(1) "3"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "19"
  }
}
gmp_rootrem(): Argument #2 ($nth) must be odd if argument #1 ($a) is negative
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(1) "0"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(1) "0"
  }
}
gmp_rootrem(): Argument #2 ($nth) must be greater than or equal to 1
gmp_rootrem(): Argument #2 ($nth) must be greater than or equal to 1
