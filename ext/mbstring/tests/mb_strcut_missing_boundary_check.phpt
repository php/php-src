--TEST--
mb_strcut() missing boundary check.
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_convert_encoding') or die("skip mb_convert_encoding() is not available in this build");
?>
--FILE--
<?php
mb_internal_encoding("UCS-4LE");
var_dump(bin2hex(mb_strcut("\x61\x00\x00\x00\x62\x00\x00\x00\x63\x00\x00\x00", 0, 32)));
mb_internal_encoding("UCS-4BE");
var_dump(bin2hex(mb_strcut("\x00\x00\x00\x61\x00\x00\x00\x62\x00\x00\x00\x63", 0, 32)));
mb_internal_encoding("UCS-2LE");
var_dump(bin2hex(mb_strcut("\x61\x00\x62\x00\x63\x00", 0, 32)));
mb_internal_encoding("UCS-2BE");
var_dump(bin2hex(mb_strcut("\x00\x61\x00\x62\x00\x63", 0, 32)));
mb_internal_encoding("UTF-16");
var_dump(bin2hex(mb_strcut("\x00\x61\x00\x62\x00\x63", 0, 32)));
mb_internal_encoding("UTF-8");
var_dump(bin2hex(mb_strcut("abc", 0, 32)));
mb_internal_encoding("ISO-8859-1");
var_dump(bin2hex(mb_strcut("abc", 0, 32)));
?>
--EXPECT--
string(24) "610000006200000063000000"
string(24) "000000610000006200000063"
string(12) "610062006300"
string(12) "006100620063"
string(12) "006100620063"
string(6) "616263"
string(6) "616263"
