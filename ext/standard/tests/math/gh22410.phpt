--TEST--
gh-22410: round() preserves integral doubles
--FILE--
<?php
var_dump(round(1.0));
var_dump(round(-1.0));
var_dump(round((float) 9003241321073828));
var_dump(round((float) -9003241321073828));
?>
--EXPECT--
float(1)
float(-1)
float(9003241321073828)
float(-9003241321073828)
