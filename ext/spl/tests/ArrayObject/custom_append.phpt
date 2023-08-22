--TEST--
Overwritting default append method
--EXTENSIONS--
spl
--FILE--
<?php

class Dummy {}

class OverWriteAppend extends ArrayObject
{
    private $data;

    function __construct($v)
    {
        $this->data = [];
        parent::__construct($v);
    }

    function append($value): void
    {
        $this->data[] = $value;
    }
}

$d = new Dummy();
$ao = new OverWriteAppend($d);

$ao->append("value1");
var_dump($ao);
?>
--EXPECT--
object(OverWriteAppend)#2 (2) {
  ["data":"OverWriteAppend":private]=>
  array(1) {
    [0]=>
    string(6) "value1"
  }
  ["storage":"ArrayObject":private]=>
  object(Dummy)#1 (0) {
  }
}
