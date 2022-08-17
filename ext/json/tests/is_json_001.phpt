--TEST--
is_json() Usage
--FILE--
<?php
var_dump(json_decode(""), is_json(""));
echo "-------------------\n";
var_dump(json_decode("."), is_json("."));
echo "-------------------\n";
var_dump(json_decode("<?>"), is_json("<?>"));
echo "-------------------\n";
var_dump(json_decode(";"), is_json(";"));
echo "-------------------\n";
var_dump(json_decode("руссиш"), is_json("руссиш"));
echo "-------------------\n";
var_dump(json_decode("blah"), is_json("blah"));
echo "-------------------\n";
var_dump(json_decode(NULL), is_json(NULL));
echo "-------------------\n";
var_dump(json_decode('{ "test": { "foo": "bar" } }'), is_json('{ "test": { "foo": "bar" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "test": { "foo": "" } }'), is_json('{ "test": { "foo": "" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "": { "foo": "" } }'), is_json('{ "": { "foo": "" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "": { "": "" } }'), is_json('{ "": { "": "" } }'));
echo "-------------------\n";
var_dump(json_decode('{ "": { "": "" }'), is_json('{ "": { "": "" }'));
echo "-------------------\n";
var_dump(json_decode('{ "": "": "" } }'), is_json('{ "": "": "" } }'));
?>

--EXPECTF--
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)
-------------------

Deprecated: json_decode(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d

Deprecated: is_json(): Passing null to parameter #1 ($json) of type string is deprecated in %s on line %d
NULL
bool(false)
-------------------
object(stdClass)#2 (1) {
  ["test"]=>
  object(stdClass)#1 (1) {
    ["foo"]=>
    string(3) "bar"
  }
}
bool(true)
-------------------
object(stdClass)#1 (1) {
  ["test"]=>
  object(stdClass)#2 (1) {
    ["foo"]=>
    string(0) ""
  }
}
bool(true)
-------------------
object(stdClass)#2 (1) {
  [""]=>
  object(stdClass)#1 (1) {
    ["foo"]=>
    string(0) ""
  }
}
bool(true)
-------------------
object(stdClass)#1 (1) {
  [""]=>
  object(stdClass)#2 (1) {
    [""]=>
    string(0) ""
  }
}
bool(true)
-------------------
NULL
bool(false)
-------------------
NULL
bool(false)