--TEST--
is_json() - Error handling
--FILE--
<?php
var_dump(is_json(null), is_json(""),is_json("", -1));

try {
  var_dump(is_json("-", 0));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
}

try {
  var_dump(is_json("-", PHP_INT_MAX));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
}

var_dump(is_json('{"key1":"value1", "key2":"value2"}', 1), is_json('{"key1":"value1", "key2":"value2"}', 2));

try {
  var_dump(is_json("-", 512, JSON_BIGINT_AS_STRING));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
}
?>

--EXPECTF--
Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
bool(false)
bool(false)
bool(false)
is_json(): Argument #2 ($depth) must be greater than 0
is_json(): Argument #2 ($depth) must be less than %d
bool(false)
bool(true)
is_json(): Argument #3 ($flags) must be a valid flag (JSON_THROW_ON_ERROR, JSON_INVALID_UTF8_IGNORE)
