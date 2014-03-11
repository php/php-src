--TEST--
gmp_root() and gmp_rootrem() basic tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_root(1000, 3));
var_dump(gmp_root(100, 3));
var_dump(gmp_root(-100, 3));

var_dump(gmp_root(1000, 4));
var_dump(gmp_root(100, 4));
var_dump(gmp_root(-100, 4));

var_dump(gmp_root(0, 3));
var_dump(gmp_root(100, 0));
var_dump(gmp_root(100, -3));

var_dump(gmp_rootrem(1000, 3));
var_dump(gmp_rootrem(100, 3));
var_dump(gmp_rootrem(-100, 3));

var_dump(gmp_rootrem(1000, 4));
var_dump(gmp_rootrem(100, 4));
var_dump(gmp_rootrem(-100, 4));

var_dump(gmp_rootrem(0, 3));
var_dump(gmp_rootrem(100, 0));
var_dump(gmp_rootrem(100, -3));

?>
--EXPECTF--	
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "10"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "4"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(2) "-4"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "5"
}
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "3"
}

Warning: gmp_root(): Can't take even root of negative number in %s on line %d
bool(false)
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "0"
}

Warning: gmp_root(): The root must be positive in %s on line %d
bool(false)

Warning: gmp_root(): The root must be positive in %s on line %d
bool(false)
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
    string(2) "36"
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

Warning: gmp_rootrem(): Can't take even root of negative number in %s on line %d
bool(false)
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

Warning: gmp_rootrem(): The root must be positive in %s on line %d
bool(false)

Warning: gmp_rootrem(): The root must be positive in %s on line %d
bool(false)
