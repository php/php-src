--TEST--
Bug #42090 (json_decode causes segmentation fault)
--FILE--
<?php
var_dump(
    json_decode('""'),
    json_decode('"..".'),
    json_decode('"'),
    json_decode('""""'),
    json_encode('"'),
    json_decode(json_encode('"')),
    json_decode(json_encode('""'))
);
?>
--EXPECT--
string(0) ""
NULL
NULL
NULL
string(4) ""\"""
string(1) """
string(2) """"
