--TEST--
Bug #46843 (CP936 euro symbol is not converted properly)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(bin2hex(mb_convert_encoding("\x80", 'UCS-2BE', 'CP936')));
var_dump(bin2hex(mb_convert_encoding("\x20\xac", 'CP936', 'UCS-2BE')));
?>
--EXPECT--
string(4) "20ac"
string(2) "80"
