--TEST--
Vector clear
--FILE--
<?php

$vec = new Vector([new stdClass()]);
var_dump($vec->toArray());
var_dump($vec->count());
$vec->clear();
foreach ($vec as $value) {
    echo "Not reached\n";
}
var_dump($vec->toArray());
var_dump($vec->count());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
int(1)
array(0) {
}
int(0)
