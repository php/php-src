--TEST--
input_get()
--GET--
a=<b>test</b>&b=http://example.com
--POST--
c=<p>string</p>&d=12345.7
--FILE--
<?php

var_dump(input_get(INPUT_GET, "a", FILTER_SANITIZE_STRIPPED));
var_dump(input_get(INPUT_GET, "b", FILTER_SANITIZE_URL));
var_dump(input_get(INPUT_GET, "a", FILTER_SANITIZE_SPECIAL_CHARS, array(1,2,3,4,5)));
var_dump(input_get(INPUT_GET, "b", FILTER_VALIDATE_FLOAT, new stdClass));
var_dump(input_get(INPUT_POST, "c", FILTER_SANITIZE_STRIPPED, array(5,6,7,8)));
var_dump(input_get(INPUT_POST, "d", FILTER_VALIDATE_FLOAT));
var_dump(input_get(INPUT_POST, "c", FILTER_SANITIZE_SPECIAL_CHARS));
var_dump(input_get(INPUT_POST, "d", FILTER_VALIDATE_INT));

var_dump(input_get(new stdClass, "d"));

var_dump(input_get(INPUT_POST, "c", "", ""));
var_dump(input_get("", "", "", "", ""));
var_dump(input_get(0, 0, 0, 0, 0));

echo "Done\n";
?>
--EXPECTF--	
string(4) "test"
string(18) "http://example.com"
string(27) "&#60;b&#62;test&#60;/b&#62;"
bool(false)
string(6) "string"
float(12345.7)
string(29) "&#60;p&#62;string&#60;/p&#62;"
bool(false)

Warning: input_get() expects parameter 1 to be long, object given in %s on line %d
NULL

Warning: input_get() expects parameter 3 to be long, string given in %s on line %d
NULL

Warning: input_get() expects parameter 1 to be long, string given in %s on line %d
NULL
NULL
Done
