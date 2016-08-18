--TEST--
Bug #69891: Unexpected array comparison result
--FILE--
<?php

var_dump([1, 2, 3] <=> []);
var_dump([] <=> [1, 2, 3]);
var_dump([1] <=> [2, 3]);
--EXPECT--
int(1)
int(-1)
int(-1)
