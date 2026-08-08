--TEST--
json_decode() with JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS - full JSONC documents
--FILE--
<?php

const JSONC_FLAGS = JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS;

$config = '{
    // server settings
    "host": "localhost", /* inline comment */
    "port": 8080,
    /*
     * feature list
     */
    "features": [
        "alpha",
        "beta", // most recent
    ],
    "debug": true,
}';

var_dump(json_decode($config, true, 512, JSONC_FLAGS));
var_dump(json_validate($config, 512, JSONC_FLAGS));

var_dump(json_decode("[1, /* c */ ]", true, 512, JSONC_FLAGS));
var_dump(json_decode("{\"a\":1, // t\n}", true, 512, JSONC_FLAGS));

// a comment between two commas does not hide the error
var_dump(json_decode("[1, // c\n,]", true, 512, JSONC_FLAGS));
echo json_last_error(), ": ", json_last_error_msg(), "\n";

?>
--EXPECT--
array(4) {
  ["host"]=>
  string(9) "localhost"
  ["port"]=>
  int(8080)
  ["features"]=>
  array(2) {
    [0]=>
    string(5) "alpha"
    [1]=>
    string(4) "beta"
  }
  ["debug"]=>
  bool(true)
}
bool(true)
array(1) {
  [0]=>
  int(1)
}
array(1) {
  ["a"]=>
  int(1)
}
NULL
4: Syntax error near location 2:1
