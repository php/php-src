--TEST--
Constant expressions with null coalescing operator ??
--FILE--
<?php

const A = [1 => [[]]];

const T_1 = null ?? A[1]['undefined']['index'] ?? 1;
const T_2 = null ?? A['undefined']['index'] ?? 2;
const T_3 = null ?? A[1][0][2] ?? 3;

var_dump(T_1);
var_dump(T_2);
var_dump(T_3);

var_dump((function(){ static $var = null ?? A[1]['undefined']['index'] ?? 1; return $var; })());
var_dump((function(){ static $var = null ?? A['undefined']['index'] ?? 2; return $var; })());
var_dump((function(){ static $var = null ?? A[1][0][2] ?? 3; return $var; })());

var_dump((new class { public $var = null ?? A[1]['undefined']['index'] ?? 1; })->var);
var_dump((new class { public $var = null ?? A['undefined']['index'] ?? 2; })->var);
var_dump((new class { public $var = null ?? A[1][0][2] ?? 3; })->var);

?>
--EXPECT--
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
