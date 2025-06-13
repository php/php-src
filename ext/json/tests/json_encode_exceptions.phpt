--TEST--
Test json_encode() function : JSON_THROW_ON_ERROR flag
--FILE--
<?php

try {
    var_dump(json_encode("\x80", JSON_THROW_ON_ERROR));
} catch (JsonException $e) {
    var_dump($e);
}

// JSON_PARTIAL_OUTPUT_ON_ERROR is incompatible with exceptions
// So it overrides it for the sake of working with wrappers that add the
// JSON_THROW_ON_ERROR flag
var_dump(json_encode("\x80", JSON_THROW_ON_ERROR | JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error());
var_dump(json_last_error_msg());

?>
--EXPECTF--
object(JsonException)#1 (7) {
  ["message":protected]=>
  string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
  ["string":"Exception":private]=>
  string(0) ""
  ["code":protected]=>
  int(5)
  ["file":protected]=>
  string(67) "/home/usuario/dev/php-src/ext/json/tests/json_encode_exceptions.php"
  ["line":protected]=>
  int(4)
  ["trace":"Exception":private]=>
  array(1) {
    [0]=>
    array(4) {
      ["file"]=>
      string(67) "/home/usuario/dev/php-src/ext/json/tests/json_encode_exceptions.php"
      ["line"]=>
      int(4)
      ["function"]=>
      string(11) "json_encode"
      ["args"]=>
      array(2) {
        [0]=>
        string(1) "%s"
        [1]=>
        int(4194304)
      }
    }
  }
  ["previous":"Exception":private]=>
  NULL
}
string(4) "null"
int(5)
string(73) "Malformed UTF-8 characters, possibly incorrectly encoded near character 0"
