--TEST--
Inc/dec of reference object properties
--FILE--
<?php

$obj = new stdClass;
$obj->cursor = 0;
$ref =& $obj->cursor;

$obj->cursor++;
var_dump($obj->cursor);

$obj->cursor--;
var_dump($obj->cursor);

?>
--EXPECT--
int(1)
int(0)
