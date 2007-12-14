--TEST--
Testing class extending to ArrayObject and serialize
--FILE--
<?php

class Name extends ArrayObject
{
    public $var = 'a';
    protected $bar = 'b';
    private $foo = 'c';
}

$a = new Name();
var_dump($a);
var_dump($a->var);

$a = unserialize(serialize($a));

var_dump($a);
var_dump($a->var);

class Sláinte extends ArrayObject
{
    public $var = 'tá';
    protected $bar = 'trí';
    private $foo = '年';
}

$a = new Sláinte();
var_dump($a);
var_dump($a->var);

$a = unserialize(serialize($a));

var_dump($a);
var_dump($a->var);

?>
--EXPECT--
object(Name)#1 (4) {
  ["var"]=>
  string(1) "a"
  ["bar":protected]=>
  string(1) "b"
  ["foo":"Name":private]=>
  string(1) "c"
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
string(1) "a"
object(Name)#2 (4) {
  ["var"]=>
  string(1) "a"
  ["bar":protected]=>
  string(1) "b"
  ["foo":"Name":private]=>
  string(1) "c"
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
string(1) "a"
object(Sláinte)#1 (4) {
  ["var"]=>
  string(3) "tá"
  ["bar":protected]=>
  string(4) "trí"
  ["foo":"Sláinte":private]=>
  string(3) "年"
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
string(3) "tá"
object(Sláinte)#2 (4) {
  ["var"]=>
  string(3) "tá"
  ["bar":protected]=>
  string(4) "trí"
  ["foo":"Sláinte":private]=>
  string(3) "年"
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
string(3) "tá"
