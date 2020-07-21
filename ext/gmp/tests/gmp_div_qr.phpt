--TEST--
gmp_div_qr() tests
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

var_dump(gmp_div_qr(0,1));
var_dump(gmp_div_qr(1,0));
var_dump(gmp_div_qr(gmp_init(1), gmp_init(0)));
var_dump(gmp_div_qr(12653,23482734));
try {
    var_dump(gmp_div_qr(12653,23482734, 10));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_div_qr(1123123,123));
var_dump(gmp_div_qr(1123123,123, 1));
var_dump(gmp_div_qr(1123123,123, 2));
var_dump(gmp_div_qr(gmp_init(1123123), gmp_init(123)));
var_dump(gmp_div_qr(1123123,123, GMP_ROUND_ZERO));
var_dump(gmp_div_qr(1123123,123, GMP_ROUND_PLUSINF));
var_dump(gmp_div_qr(1123123,123, GMP_ROUND_MINUSINF));

$fp = fopen(__FILE__, 'r');

var_dump(gmp_div_qr($fp, $fp));
var_dump(gmp_div_qr(array(), array()));

echo "Done\n";
?>
--EXPECTF--
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

Warning: gmp_div_qr(): Zero operand not allowed in %s on line %d
bool(false)

Warning: gmp_div_qr(): Zero operand not allowed in %s on line %d
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
    string(5) "12653"
  }
}
gmp_div_qr(): Argument #3 ($round) must be one of GMP_ROUND_ZERO, GMP_ROUND_PLUSINF, or GMP_ROUND_MINUSINF
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9131"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "10"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9132"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "-113"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9131"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "10"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9131"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "10"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9131"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "10"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9132"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "-113"
  }
}
array(2) {
  [0]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(4) "9131"
  }
  [1]=>
  object(GMP)#%d (1) {
    ["num"]=>
    string(2) "10"
  }
}

Warning: gmp_div_qr(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)

Warning: gmp_div_qr(): Unable to convert variable to GMP - wrong type in %s on line %d
bool(false)
Done
