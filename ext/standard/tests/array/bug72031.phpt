--TEST--
Bug #72031: array_column() against an array of objects discards all values matching null
--FILE--
<?php

class myObj {
    public $prop;
    public function __construct($prop) {
        $this->prop = $prop;
    }
}

$objects = [
    new myObj(-1),
    new myObj(0),
    new myObj(1),
    new myObj(2),
    new myObj(null),
    new myObj(true),
    new myObj(false),
    new myObj('abc'),
    new myObj(''),
];

var_dump(array_column($objects, 'prop'));

?>
--EXPECT--
array(9) {
  [0]=>
  int(-1)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  int(2)
  [4]=>
  NULL
  [5]=>
  bool(true)
  [6]=>
  bool(false)
  [7]=>
  string(3) "abc"
  [8]=>
  string(0) ""
}
