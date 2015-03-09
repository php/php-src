--TEST--
isset() can be used on dereferences of temporary expressions
--FILE--
<?php

var_dump(isset([0, 1][0]));
var_dump(isset(([0, 1] + [])[0]));
var_dump(isset([[0, 1]][0][0]));
var_dump(isset(([[0, 1]] + [])[0][0]));
var_dump(isset(((object) ['a' => 'b'])->a));
var_dump(isset(['a' => 'b']->a));
var_dump(isset("str"->a));
var_dump(isset((['a' => 'b'] + [])->a));
var_dump(isset((['a' => 'b'] + [])->a->b));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
