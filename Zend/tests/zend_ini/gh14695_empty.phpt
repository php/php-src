--TEST--
GH-14695: Empty upload_max_filesize value is rejected
--INI--
upload_max_filesize=
--FILE--
<?php

var_dump(ini_get('upload_max_filesize'));

?>
--EXPECTF--
Warning: Invalid "upload_max_filesize" setting. Invalid quantity "": no valid leading digits in %s on line %d
string(2) "2M"
