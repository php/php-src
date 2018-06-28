--TEST--
__equals: Converts to boolean on return
--FILE--
<?php

var_dump(new class { public function __equals($other) { return 0;       } } == 1);
var_dump(new class { public function __equals($other) { return -0;      } } == 1);
var_dump(new class { public function __equals($other) { return 1;       } } == 1);
var_dump(new class { public function __equals($other) { return -1;      } } == 1);
var_dump(new class { public function __equals($other) { return 0.5;     } } == 1);
var_dump(new class { public function __equals($other) { return -0.5;    } } == 1);
var_dump(new class { public function __equals($other) { return 'a';     } } == 1);
var_dump(new class { public function __equals($other) { return '0';     } } == 1);
var_dump(new class { public function __equals($other) {                 } } == 1);

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
