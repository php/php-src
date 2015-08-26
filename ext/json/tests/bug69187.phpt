--TEST--
Bug #69187 json_last_error return BC in PHP7
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
var_dump(json_decode(NULL)); 
var_dump(json_last_error());
var_dump(json_decode(FALSE));
var_dump(json_last_error());
var_dump(json_decode(""));
var_dump(json_last_error());

var_dump(json_decode(0));
var_dump(json_last_error());
var_dump(json_decode(1));
var_dump(json_last_error());
var_dump(json_decode(TRUE));
var_dump(json_last_error());

json_decode("\xED\xA0\xB4");
var_dump(json_last_error());

json_decode("\x00");
var_dump(json_last_error());

json_decode("\"\xED\xA0\xB4\"");
var_dump(json_last_error());

json_decode("\"\x00\"");
var_dump(json_last_error());
?>
--EXPECT--
NULL
int(4)
NULL
int(4)
NULL
int(4)
int(0)
int(0)
int(1)
int(0)
int(1)
int(0)
int(5)
int(3)
int(5)
int(3)
