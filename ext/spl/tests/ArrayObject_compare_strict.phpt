--TEST--
SPL: ArrayObject: `==` comparison with strict_operators directive
--FILE--
<?php
declare(strict_operators=1);

$o = new ArrayObject(['foo' => 0, 'bar' => 2]);
$x = new ArrayObject(['bar' => 2, 'foo' => 0]);
$y = new ArrayObject(['foo' => null, 'bar' => 2]);
$z = new ArrayObject(['foo' => 0, 'bar' => '2']);
$nop = new ArrayObject(['foo' => 99, 'bar' => 99]);

var_dump($o == $nop);
var_dump($o == $x);
var_dump($o == $y);
var_dump($o == $z);
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(false)