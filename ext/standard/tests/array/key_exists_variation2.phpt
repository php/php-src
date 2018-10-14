--TEST--
Test function key_exists() by calling it with its expected arguments
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
echo "*** test key_exists() by using mixed type of arrays ***\n";

// there is not a index = 0 element
$a = array(1 => 'bar', 'foo' => 'baz');
var_dump(key_exists(0, $a));

echo "integer\n";
// 1 has index = 0
$b = array(1, 'foo' => 'baz');
var_dump(key_exists(0, $b));

// 42 has index = 0, netherless its position is the latest
$c = array('foo' => 'baz', 42);
var_dump(key_exists(0, $c));

echo "string\n";
// 'bar' has index = 0, netherless it is a string
$d = array('bar', 'foo' => 'baz');
var_dump(key_exists(0, $d));

// 'baz' has index = 0, netherless its position is the latest
$e = array('foo' => 'baz', 'baz');
var_dump(key_exists(0, $e));

echo "obj\n";
class ObjectA
{
  public $foo = 'bar';
}

$obj = new ObjectA();

// object has index = 0, netherless its position is the latest
$f = array('foo' => 'baz', $obj);
var_dump(key_exists(0, $f));

// object has index = 0, netherless its position is the first
$g = array($obj, 'foo' => 'baz');
var_dump(key_exists(0, $g));

echo "stream resource\n";
// stream resource has index = 0, netherless its position is the first
$st = fopen('php://memory', '+r');
$h = array($st, 'foo' => 'baz');
var_dump(key_exists(0, $h));

// stream resource has index = 0, netherless its position is the latest
$i = array('foo' => 'baz', $st);
var_dump(key_exists(0, $i));
--EXPECTF--
*** test key_exists() by using mixed type of arrays ***
bool(false)
integer
bool(true)
bool(true)
string
bool(true)
bool(true)
obj
bool(true)
bool(true)
stream resource
bool(true)
bool(true)
