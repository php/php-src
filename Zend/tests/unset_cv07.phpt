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
--EXPECTF--
1
<br />
<b>Notice</b>:  import_request_variables() [<a href='function.import-request-variables'>function.import-request-variables</a>]: No prefix specified - possible security hazard in <b>%sunset_cv07.php</b> on line <b>4</b><br />
%sunset_cv07.php(4) : Notice - import_request_variables() [<a href='function.import-request-variables'>function.import-request-variables</a>]: No prefix specified - possible security hazard
2
ok
