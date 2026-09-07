--TEST--
Missing separation for ASSIGN_DIM with ref OP_DATA
--FILE--
<?php

function a() {
    $a[] = $b[] = &$a;
    var_dump($a, $b);
}

function b() {
    $a = [];
    $b = [];
    $a[] = $b[] = &$a;
    var_dump($a, $b);
}

function c() {
    $a[] = $b = &$a;
    var_dump($a, $b);
}

function d() {
    $a = $b[] = &$a;
    var_dump($a, $b);
}

a();
b();
c();
d();

?>
--EXPECT--
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    NULL
  }
}
array(1) {
  [0]=>
  array(0) {
  }
}
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    array(0) {
    }
  }
}
array(1) {
  [0]=>
  NULL
}
array(1) {
  [0]=>
  NULL
}
NULL
array(1) {
  [0]=>
  &NULL
}
