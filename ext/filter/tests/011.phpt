--TEST--
input_get()
--GET--
a=<b>test</b>&b=http://example.com
--POST--
c=<p>string</p>&d=12345.7
--FILE--
<?php

var_dump(input_get(INPUT_GET, "a", FS_STRIPPED));
var_dump(input_get(INPUT_GET, "b", FS_URL));
var_dump(input_get(INPUT_GET, "a", FS_SPECIAL_CHARS, array(1,2,3,4,5)));
var_dump(input_get(INPUT_GET, "b", FL_FLOAT, new stdClass));
var_dump(input_get(INPUT_POST, "c", FS_STRIPPED, array(5,6,7,8)));
var_dump(input_get(INPUT_POST, "d", FL_FLOAT));
var_dump(input_get(INPUT_POST, "c", FS_SPECIAL_CHARS));
var_dump(input_get(INPUT_POST, "d", FL_INT));

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
NULL
string(6) "string"
float(12345.7)
string(29) "&#60;p&#62;string&#60;/p&#62;"
NULL
PHP Warning:  input_get() expects parameter 1 to be long, object given in %s on line %d

Warning: input_get() expects parameter 1 to be long, object given in %s on line %d
NULL
PHP Warning:  input_get() expects parameter 3 to be long, string given in %s on line %d

Warning: input_get() expects parameter 3 to be long, string given in %s on line %d
NULL
PHP Warning:  input_get() expects parameter 1 to be long, string given in %s on line %d

Warning: input_get() expects parameter 1 to be long, string given in %s on line %d
NULL
bool(false)
Done
