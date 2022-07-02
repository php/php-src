--TEST--
decrementing different variables
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--INI--
precision=14
--FILE--
<?php

$a = array(
    array(1,2,3),
    "",
    1,
    2.5,
    0,
    "string",
    "123",
    "2.5",
    NULL,
    true,
    false,
    new stdclass,
    array(),
    -PHP_INT_MAX-1,
    (string)(-PHP_INT_MAX-1),
);

foreach ($a as $var) {
    try {
        $var--;
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
Cannot decrement array
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
int(-1)
int(0)
float(1.5)
int(-1)
string(6) "string"
int(122)
float(1.5)
NULL
bool(true)
bool(false)
Cannot decrement stdClass
object(stdClass)#%d (0) {
}
Cannot decrement array
array(0) {
}
float(-2147483649)
float(-2147483649)
Done
