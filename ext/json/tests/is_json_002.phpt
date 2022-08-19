--TEST--
is_json() - Error handling
--FILE--
<?php
var_dump(
  is_json(null),
  json_last_error(),
  json_last_error_msg(),
  
  is_json(""),
  json_last_error(),
  json_last_error_msg(),

  is_json("", -1),
  json_last_error(),
  json_last_error_msg()
);

try {
  var_dump(is_json("-", 0));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
  var_dump(json_last_error(), json_last_error_msg());
}

try {
  var_dump(is_json("-", PHP_INT_MAX));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
  var_dump(json_last_error(), json_last_error_msg());
}

var_dump(
  is_json('{"key1":"value1", "key2":"value2"}', 1),
  json_last_error(), json_last_error_msg(),

  is_json('{"key1":"value1", "key2":"value2"}', 2),
  json_last_error(), json_last_error_msg()
);

try {
  var_dump(is_json("-", 512, JSON_BIGINT_AS_STRING));
} catch (Error $error) {
  echo $error->getMessage() . PHP_EOL;
  var_dump(json_last_error(), json_last_error_msg());
}

try {
  is_json("-", 512, JSON_THROW_ON_ERROR);
} catch (Exception $exception) {
  echo $exception->getMessage() . PHP_EOL;
  var_dump(
    json_last_error(),
    json_last_error_msg()
  );
}

echo PHP_EOL . "Testing Invalid UTF-8-" . PHP_EOL;

/*var_dump(
  is_json("\"a\xb0b\""),
  is_json("\"a\xd0\xf2b\""),
  is_json("\"\x61\xf0\x80\x80\x41\""),
  is_json("[\"\xc1\xc1\",\"a\"]"),

  is_json("\"a\xb0b\"", 512, JSON_INVALID_UTF8_IGNORE),
  is_json("\"a\xd0\xf2b\"", 512, JSON_INVALID_UTF8_IGNORE),
  is_json("\"\x61\xf0\x80\x80\x41\"", 512, JSON_INVALID_UTF8_IGNORE),
  is_json("[\"\xc1\xc1\",\"a\"]", 512, JSON_INVALID_UTF8_IGNORE),
);*/
?>

--EXPECTF--
Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
bool(false)
int(4)
string(12) "Syntax error"
is_json(): Argument #2 ($depth) must be greater than 0
int(0)
string(8) "No error"
is_json(): Argument #2 ($depth) must be less than 2147483647
int(0)
string(8) "No error"
bool(false)
int(1)
string(28) "Maximum stack depth exceeded"
bool(true)
int(0)
string(8) "No error"
is_json(): Argument #3 ($flags) must be a valid flag (JSON_THROW_ON_ERROR, JSON_INVALID_UTF8_IGNORE)
int(0)
string(8) "No error"
Syntax error
int(0)
string(8) "No error"

Testing Invalid UTF-8-
