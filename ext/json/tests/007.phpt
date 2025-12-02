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
string(46) "Maximum stack depth exceeded near location 1:2"
NULL
int(2)
string(60) "State mismatch (invalid or malformed JSON) near location 1:3"
NULL
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:2"
NULL
int(4)
string(30) "Syntax error near location 1:3"
Done
