--TEST--
swap function test
--FILE--
<?php
$value1 = 2;
$value2 = 'abc';

var_dump(swap($value1, $value2));
var_dump($value1);
var_dump($value2);

class obj1 {
  public $value = 2;
}

class obj2 {
  public $value = 'abc';
}

$value1 = new obj1;
$value2 = new obj2;

var_dump(swap($value1, $value2));
var_dump($value1->value);
var_dump($value2->value);

var_dump(swap());
var_dump(swap($value1, $value2, $value2));
?>
--EXPECT--
bool(true)
string(3) "abc"
int(2)
bool(true)
string(3) "abc"
int(2)
bool(false)
bool(false)