--TEST--
Test INI internal_encoding basic - invalid encoding specified in INI
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
?>
--INI--
internal_encoding=BAD
--FILE--
<?php

echo "*** Testing INI internal_encoding: invalid encoding specified in INI ***\n";

echo mb_internal_encoding()."\n";
mb_internal_encoding('UTF-8');
echo mb_internal_encoding()."\n";

?>
--EXPECT--
*** Testing INI internal_encoding: invalid encoding specified in INI ***
UTF-8
UTF-8
