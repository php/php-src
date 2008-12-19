--TEST--
json_decode() tests
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_decode());
var_dump(json_decode(""));
var_dump(json_decode("", 1));
var_dump(json_decode("", 0));
var_dump(json_decode(".", 1));
var_dump(json_decode(".", 0));
var_dump(json_decode("<?>"));
var_dump(json_decode(";"));
var_dump(json_decode("руссиш"));
var_dump(json_decode("blah"));
var_dump(json_decode(NULL));
var_dump(json_decode('[1}'));
var_dump(json_decode('{ "test": { "foo": "bar" } }'));
var_dump(json_decode('{ "test": { "foo": "" } }'));
var_dump(json_decode('{ "": { "foo": "" } }'));
var_dump(json_decode('{ "": { "": "" } }'));
var_dump(json_decode('{ "": { "": "" }'));
var_dump(json_decode('{ "": "": "" } }'));

echo "Done\n";
?>
--EXPECTF--
Warning: json_decode() expects at least 1 parameter, 0 given in %s on line %d
NULL
NULL
NULL
NULL
string(1) "."
string(1) "."
string(3) "<?>"
string(1) ";"
string(12) "руссиш"
string(4) "blah"
NULL
NULL
object(stdClass)#1 (1) {
  ["test"]=>
  object(stdClass)#2 (1) {
    ["foo"]=>
    string(3) "bar"
  }
}
object(stdClass)#1 (1) {
  ["test"]=>
  object(stdClass)#2 (1) {
    ["foo"]=>
    string(0) ""
  }
}
object(stdClass)#1 (1) {
  ["_empty_"]=>
  object(stdClass)#2 (1) {
    ["foo"]=>
    string(0) ""
  }
}
object(stdClass)#1 (1) {
  ["_empty_"]=>
  object(stdClass)#2 (1) {
    ["_empty_"]=>
    string(0) ""
  }
}
NULL
NULL
Done
