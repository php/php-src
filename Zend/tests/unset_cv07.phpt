--TEST--
unset() CV 7 (indirect unset() of global variable in import_request_variables())
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--INI--
error_reporting=2039
--GET--
x=2
--FILE--
<?php
$x = "1\n";
echo $x;
import_request_variables("g");
echo $x;
echo "\nok\n";
?>
--EXPECT--
1
2
ok
