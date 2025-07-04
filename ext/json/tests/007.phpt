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
string(28) "Maximum stack depth exceeded"
NULL
int(2)
string(42) "State mismatch (invalid or malformed JSON)"
NULL
int(3)
string(70) "Control character error, possibly incorrectly encoded near character 2"
NULL
int(4)
string(29) "Syntax error near character 0"
Done
