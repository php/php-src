--TEST--
SPL: iterator_count() supports arrays.
--FILE--
<?php

var_dump(iterator_count([]));
var_dump(iterator_count([1]));
var_dump(iterator_count(['a' => 1, 'b' => 2, 5 => 3]));

?>
--EXPECT--
int(0)
int(1)
int(3)
