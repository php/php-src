--TEST--
json_last_error() tests
--FILE--
<?php
var_dump(json_decode("[1]"));
var_dump(json_last_error(), json_last_error_msg());
var_dump(json_decode("[[1]]", false, 2));
var_dump(json_last_error(), json_last_error_msg());
var_dump(json_decode("[1}"));
var_dump(json_last_error(), json_last_error_msg());
var_dump(json_decode('["' . chr(0) . 'abcd"]'));
var_dump(json_last_error(), json_last_error_msg());
var_dump(json_decode("[1"));
var_dump(json_last_error(), json_last_error_msg());

echo "Done\n";
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
int(0)
string(8) "No error"
NULL
int(1)
string(63) "Maximum stack depth exceeded, at character 1 near content: [1]]"
NULL
int(2)
string(74) "State mismatch (invalid or malformed JSON), at character 2 near content: }"
NULL
int(3)
string(85) "Control character error, possibly incorrectly encoded, at character 1 near content: ""
NULL
int(4)
string(12) "Syntax error"
Done
