--TEST--
casting different variables to object using settype()
--FILE--
<?php

$r = fopen(__FILE__, "r");

class test {
    function  __toString() {
        return "10";
    }
}

$o = new test;

$vars = array(
    "string",
    "8754456",
    "",
    "\0",
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
    settype($var, "object");
    var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  string(6) "string"
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  string(7) "8754456"
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  string(0) ""
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  string(1) "%0"
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  int(9876545)
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  float(0.1)
}
object(DynamicObject)#2 (0) {
}
object(DynamicObject)#2 (3) {
  ["0"]=>
  int(1)
  ["1"]=>
  int(2)
  ["2"]=>
  int(3)
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  bool(false)
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  bool(true)
}
object(DynamicObject)#2 (0) {
}
object(DynamicObject)#2 (1) {
  ["scalar"]=>
  resource(%d) of type (stream)
}
object(test)#%d (0) {
}
Done
