--TEST--
Bug #29015 (Incorrect behavior of member vars(non string ones)-numeric mem vars und others)
--FILE--
<?php
$a = new stdClass();
$x = "";
$a->$x = "string('')";
var_dump($a);
$a->{"\0"} = 42;
var_dump($a);
?>
--EXPECTF--
object(stdClass)#1 (1) {
  [""]=>
  string(10) "string('')"
}

Fatal error: Uncaught Error: Cannot access property started with '\0' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
