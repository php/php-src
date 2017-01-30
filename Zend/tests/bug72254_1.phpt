--TEST--
bug #72254 - simple test
--FILE--
<?php
$a = new stdClass();
$a->{1} = 5;
var_dump($a);
$b =get_object_vars($a);
var_dump($b, $b[1], $b["1"], reset($b), key($b));
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
object(stdClass)#1 (1) {
  ["1"]=>
  int(5)
}
array(1) {
  [1]=>
  int(5)
}
int(5)
int(5)
int(5)
int(1)
===DONE===
