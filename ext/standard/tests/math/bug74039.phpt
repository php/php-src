--TEST--
Bug #74039: is_infinite(-INF) returns false
--FILE--
<?php

var_dump(is_finite(INF));
var_dump(is_infinite(INF));
var_dump(is_nan(INF));

var_dump(is_finite(-INF));
var_dump(is_infinite(-INF));
var_dump(is_nan(-INF));

var_dump(is_finite(NAN));
var_dump(is_infinite(NAN));
var_dump(is_nan(NAN));

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
