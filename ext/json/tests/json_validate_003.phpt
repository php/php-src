--TEST--
json_validate() - Error handling for max depth
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

try {
  var_dump(json_validate("-", PHP_INT_MAX));
} catch (Throwable $error) {
  echo $error::class, ': ', $error->getMessage(), "\n";
  var_dump(json_last_error(), json_last_error_msg());
}

?>
--EXPECTF--
ValueError: json_validate(): Argument #2 ($depth) must be less than %d
int(0)
string(8) "No error"
