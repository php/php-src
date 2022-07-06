--TEST--
Bug #62653: unset($array[$float]) causes a crash
--FILE--
<?php
$array = array("5"=>"bar");
$foo = "10.0000"; // gettype($foo) = "string"
$foo /= 2; //Makes $foo = 5 but still gettype($foo) = "double"
unset($array[$foo]);
print_r($array);

$array = array("5"=>"bar");
$foo = "5";
unset($array[(float)$foo]);
print_r($array);

$array = array("5"=>"bar");
$foo = "10.0000";
$foo /= 2; //Makes $foo = 5 but still gettype($foo) = "double"
$name = "foo";
unset($array[$$name]);
print_r($array);

?>
--EXPECT--
Array
(
)
Array
(
)
Array
(
)
