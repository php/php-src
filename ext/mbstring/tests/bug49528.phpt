--TEST--
Bug #49528 (UTF-16 strings prefixed by BOM wrongly converted)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(bin2hex(mb_convert_encoding("\xff\xfe\x01\x02\x03\x04", "UCS-2BE", "UTF-16")));
var_dump(bin2hex(mb_convert_encoding("\xfe\xff\x01\x02\x03\x04", "UCS-2BE", "UTF-16")));
var_dump(bin2hex(mb_convert_encoding("\xff\xfe\xff\xfe\x01\x02\x03\x04", "UCS-2BE", "UTF-16")));
var_dump(bin2hex(mb_convert_encoding("\xff\xfe\xfe\xff\x01\x02\x03\x04", "UCS-2BE", "UTF-16")));
var_dump(bin2hex(mb_convert_encoding("\xfe\xff\xff\xfe\x01\x02\x03\x04", "UCS-2BE", "UTF-16")));
var_dump(bin2hex(mb_convert_encoding("\xfe\xff\xfe\xff\x01\x02\x03\x04", "UCS-2BE", "UTF-16")));
?>
--EXPECT--
string(8) "02010403"
string(8) "01020304"
string(12) "feff02010403"
string(12) "fffe02010403"
string(12) "fffe01020304"
string(12) "feff01020304"
