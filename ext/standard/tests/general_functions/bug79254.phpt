--TEST--
Bug #79254 (getenv() w/o arguments not showing changes)
--FILE--
<?php

$old = getenv();
var_dump(getenv("PHP_BUG_79254", true));

putenv("PHP_BUG_79254=BAR");

$new = getenv();
var_dump(array_diff($new, $old));
var_dump(getenv("PHP_BUG_79254", true));

?>
--EXPECT--
bool(false)
array(1) {
  ["PHP_BUG_79254"]=>
  string(3) "BAR"
}
string(3) "BAR"
