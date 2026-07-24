--TEST--
json_decode() with JSON_ALLOW_TRAILING_COMMAS
--FILE--
<?php

function decode_dump(string $json, int $flags = JSON_ALLOW_TRAILING_COMMAS, ?bool $assoc = true) {
    var_dump(json_decode($json, $assoc, 512, $flags));
    echo json_last_error(), ": ", json_last_error_msg(), "\n";
}

decode_dump("[1,]");
decode_dump("[1, ]");
decode_dump("[1,\n]");
decode_dump("[\"a\",]");
decode_dump("{\"a\":1,}");
decode_dump("{\"a\":1,}", JSON_ALLOW_TRAILING_COMMAS, false);
decode_dump("[[1,],]");
decode_dump("{\"a\":[1,],}");
// a lone or repeated comma is still an error
decode_dump("[,]");
decode_dump("{,}");
decode_dump("[,1]");
decode_dump("{,\"a\":1}");
decode_dump("[1,,]");
decode_dump("[1,,2]");
decode_dump("{\"a\":1,,}");
// mismatched closer after a trailing comma
decode_dump("{\"a\":1,]");
decode_dump("[1,}");
// a comment between the opener and a lone comma does not make it valid
decode_dump("[/*c*/,]", JSON_ALLOW_COMMENTS | JSON_ALLOW_TRAILING_COMMAS);
// trailing commas and JSON_THROW_ON_ERROR
try {
    var_dump(json_decode("[1,]", true, 512, JSON_ALLOW_TRAILING_COMMAS | JSON_THROW_ON_ERROR));
} catch (JsonException $e) {
    echo "JsonException: ", $e->getCode(), " ", $e->getMessage(), "\n";
}
try {
    json_decode("[1,,]", true, 512, JSON_ALLOW_TRAILING_COMMAS | JSON_THROW_ON_ERROR);
} catch (JsonException $e) {
    echo "JsonException: ", $e->getCode(), " ", $e->getMessage(), "\n";
}
// validate mirrors decode
var_dump(json_validate("[1,]", 512, JSON_ALLOW_TRAILING_COMMAS));
var_dump(json_validate("{\"a\":1,}", 512, JSON_ALLOW_TRAILING_COMMAS));
var_dump(json_validate("[1,,]", 512, JSON_ALLOW_TRAILING_COMMAS));

?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
0: No error
array(1) {
  [0]=>
  int(1)
}
0: No error
array(1) {
  [0]=>
  int(1)
}
0: No error
array(1) {
  [0]=>
  string(1) "a"
}
0: No error
array(1) {
  ["a"]=>
  int(1)
}
0: No error
object(stdClass)#%d (1) {
  ["a"]=>
  int(1)
}
0: No error
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}
0: No error
array(1) {
  ["a"]=>
  array(1) {
    [0]=>
    int(1)
  }
}
0: No error
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:2
NULL
4: Syntax error near location 1:4
NULL
4: Syntax error near location 1:4
NULL
4: Syntax error near location 1:8
NULL
2: State mismatch (invalid or malformed JSON) near location 1:8
NULL
2: State mismatch (invalid or malformed JSON) near location 1:4
NULL
4: Syntax error near location 1:7
array(1) {
  [0]=>
  int(1)
}
JsonException: 4 Syntax error near location 1:4
bool(true)
bool(true)
bool(false)
