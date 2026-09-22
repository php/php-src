--TEST--
casting different variables to resource using settype()
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
    try {
        settype($var, "resource");
    } catch (Throwable $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
    var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
ValueError: Cannot convert to resource type
string(6) "string"
ValueError: Cannot convert to resource type
string(7) "8754456"
ValueError: Cannot convert to resource type
string(0) ""
ValueError: Cannot convert to resource type
string(1) "%0"
ValueError: Cannot convert to resource type
int(9876545)
ValueError: Cannot convert to resource type
float(0.1)
ValueError: Cannot convert to resource type
array(0) {
}
ValueError: Cannot convert to resource type
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
ValueError: Cannot convert to resource type
bool(false)
ValueError: Cannot convert to resource type
bool(true)
ValueError: Cannot convert to resource type
NULL
ValueError: Cannot convert to resource type
resource(%d) of type (stream)
ValueError: Cannot convert to resource type
object(test)#%d (0) {
}
Done
