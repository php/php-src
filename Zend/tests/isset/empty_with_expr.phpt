--TEST--
empty() with arbitrary expressions
--FILE--
<?php

function getEmptyArray() { return []; }
function getNonEmptyArray() { return [1, 2, 3]; }

var_dump(empty([]));
var_dump(empty([1, 2, 3]));

var_dump(empty(getEmptyArray()));
var_dump(empty(getNonEmptyArray()));

var_dump(empty([] + []));
var_dump(empty([1, 2, 3] + []));

var_dump(empty("string"));
var_dump(empty(""));
var_dump(empty(true));
var_dump(empty(false));
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
