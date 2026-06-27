--TEST--
GH-14695: Invalid upload_max_filesize and post_max_size values are rejected
--INI--
upload_max_filesize=1zz
post_max_size=
--FILE--
<?php

var_dump(ini_get('upload_max_filesize'));
var_dump(ini_get('post_max_size'));

?>
--EXPECTF--
Warning: Invalid "upload_max_filesize" setting. Invalid quantity "1zz": unknown multiplier "z" in %s on line %d

Warning: Invalid "post_max_size" setting. Invalid quantity "": no valid leading digits in %s on line %d
string(2) "2M"
string(2) "8M"
