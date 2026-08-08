--TEST--
json_decode() with JSON_ALLOW_COMMENTS - valid comment usage
--FILE--
<?php

function decode_dump(string $json) {
    var_dump(json_decode($json, true, 512, JSON_ALLOW_COMMENTS));
    if (json_last_error() !== JSON_ERROR_NONE) {
        echo "error: ", json_last_error_msg(), "\n";
    }
}

decode_dump("// comment\n1");
decode_dump("1 // comment at EOF without newline");
decode_dump("/* comment */ 1");
decode_dump("1/*c*/");
decode_dump("/**/1");
decode_dump("/***/1");
decode_dump("/*/*/1");
decode_dump("/* /* */1");
decode_dump("[1, // a\n2]");
decode_dump("{\"a\" /*x*/ : /*y*/ 1 /*z*/, // t\n \"b\":2}");
decode_dump('"// not a comment"');
decode_dump('"/* not a comment */"');
decode_dump("//only\n// comments\n[]");
// empty containers whose only content is a comment
decode_dump("[/*x*/]");
decode_dump("{/*x*/}");
decode_dump("[ //x\n]");
decode_dump("{ //x\n}");
// CRLF and bare CR both terminate a line comment
decode_dump("// crlf\r\n[1, 2]");
decode_dump("// bare cr\r1");
// comment bodies are opaque: bytes that are not valid UTF-8 are skipped
decode_dump("/* \xFF */1");
var_dump(json_validate("// c\n{\"a\": [1, 2]}", 512, JSON_ALLOW_COMMENTS));
var_dump(json_validate("{/*x*/}", 512, JSON_ALLOW_COMMENTS));
var_dump(json_validate("/* \xFF */{}", 512, JSON_ALLOW_COMMENTS));

?>
--EXPECT--
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
string(16) "// not a comment"
string(19) "/* not a comment */"
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
int(1)
int(1)
bool(true)
bool(true)
bool(true)
