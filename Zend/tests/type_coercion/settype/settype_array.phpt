--TEST--
casting different variables to array using settype()
--FILE--
<?php

$r = fopen(__FILE__, "r");

class test {
    private $var1 = 1;
    public $var2 = 2;
    protected $var3 = 3;

    function  __toString() {
        return "10";
    }
}

$o = new test;

$vars = array(
    "string",
    "",
    "\0",
    "8754456",
    9876545,
    0.10,
    array(),
    array(1,2,3),
    false,
    true,
    NULL,
    $r,
    $o
);

foreach ($vars as $var) {
    settype($var, "array");
    var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
array(1) {
  [0]=>
  string(6) "string"
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(1) "%0"
}
array(1) {
  [0]=>
  string(7) "8754456"
}
array(1) {
  [0]=>
  int(9876545)
}
array(1) {
  [0]=>
  float(0.1)
}
array(0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(1) {
  [0]=>
  bool(false)
}
array(1) {
  [0]=>
  bool(true)
}
array(0) {
}
array(1) {
  [0]=>
  resource(%d) of type (stream)
}
array(3) {
  ["%0test%0var1"]=>
  int(1)
  ["var2"]=>
  int(2)
  ["%0*%0var3"]=>
  int(3)
}
Done
