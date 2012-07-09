--TEST--
json_last_error() tests
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
var_dump(json_decode("[1]"));
var_dump(json_last_error());
var_dump(json_decode("[[1]]", false, 2));
var_dump(json_last_error());
var_dump(json_decode("[1}"));
var_dump(json_last_error());
var_dump(json_decode('["' . chr(0) . 'abcd"]'));
var_dump(json_last_error());
var_dump(json_decode("[1"));
var_dump(json_last_error());


echo "Done\n";
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
int(0)
NULL
int(1)
NULL
int(2)
NULL
int(3)
NULL
int(4)
Done

