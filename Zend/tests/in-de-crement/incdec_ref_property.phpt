--TEST--
Incrementing and decrementing a referenced property
--FILE--
<?php

$obj = new stdClass;
$obj->prop = 1;
$ref =& $obj->prop;
var_dump(++$obj->prop);
var_dump($obj->prop);
var_dump($obj->prop++);
var_dump($obj->prop);
var_dump(--$obj->prop);
var_dump($obj->prop);
var_dump($obj->prop--);
var_dump($obj->prop);

?>
--EXPECT--
int(2)
int(2)
int(2)
int(3)
int(2)
int(2)
int(2)
int(1)
