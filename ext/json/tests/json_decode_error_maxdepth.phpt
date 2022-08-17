--TEST--
Test json_decode() function : over max depth allowed
--FILE--
<?php
try {
  var_dump(json_decode("{}", true, PHP_INT_MAX));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
  var_dump(json_last_error(), json_last_error_msg());
}
?>
--EXPECTF--
json_decode(): Argument #3 ($depth) must be less than %d
int(0)
string(8) "No error"
