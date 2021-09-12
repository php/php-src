--TEST--
Vector clear
--FILE--
<?php

$it = new Vector([new stdClass()]);
var_dump($it->toArray());
var_dump($it->count());
var_dump($it->capacity());
$it->clear();
foreach ($it as $value) {
    echo "Not reached\n";
}
var_dump($it->toArray());
var_dump($it->count());
var_dump($it->capacity());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
int(1)
int(1)
array(0) {
}
int(0)
int(0)
