--TEST--
Fixed execute_data corruption with __toString()
--FILE--
<?php

class Test
{
  public $something = 'hello';

  public function __toString()
  {
    return $this->something;
  }
}

$t = new Test;
var_dump(strlen($t));
var_dump($t->something);

class Test2
{
  public $something;

  public function __construct(&$a)
  {
    $this->something = &$a;
  }

  public function __toString()
  {
    return $this->something;
  }
}

$a = 'world';
$t2 = new Test2($a);
var_dump(strlen($t2));
var_dump($t2->something);
var_dump($a);

$a = 'foobar';
var_dump(strlen($t2));
var_dump($t2->something);
--EXPECT--
int(5)
string(5) "hello"
int(5)
string(5) "world"
string(5) "world"
int(6)
string(6) "foobar"
