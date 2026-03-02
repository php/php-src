--TEST--
GH-19300 (Nested array_multisort invocation with error breaks) - correct invocation variation
--FILE--
<?php
class MyStringable {
    public function __construct(private string $data) {}
    public function __tostring() {
        array_multisort([]); // Trigger update of array sort globals in happy path
        return $this->data;
    }
}

$inputs = [
    new MyStringable('3'),
    new MyStringable('1'),
    new MyStringable('2'),
];

var_dump(array_multisort($inputs, SORT_STRING));
var_dump($inputs);
?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  object(MyStringable)#2 (1) {
    ["data":"MyStringable":private]=>
    string(1) "1"
  }
  [1]=>
  object(MyStringable)#3 (1) {
    ["data":"MyStringable":private]=>
    string(1) "2"
  }
  [2]=>
  object(MyStringable)#1 (1) {
    ["data":"MyStringable":private]=>
    string(1) "3"
  }
}
