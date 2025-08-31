--TEST--
ZE2 object cloning, 1
--FILE--
<?php
class test {
    public $p1 = 1;
    public $p2 = 2;
    public $p3;
};

$obj = new test;
$obj->p2 = 'A';
$obj->p3 = 'B';
$copy = clone $obj;
$copy->p3 = 'C';
echo "Object\n";
var_dump($obj);
echo "Clown\n";
var_dump($copy);
echo "Done\n";
?>
--EXPECT--
Object
object(test)#1 (3) {
  ["p1"]=>
  int(1)
  ["p2"]=>
  string(1) "A"
  ["p3"]=>
  string(1) "B"
}
Clown
object(test)#2 (3) {
  ["p1"]=>
  int(1)
  ["p2"]=>
  string(1) "A"
  ["p3"]=>
  string(1) "C"
}
Done
