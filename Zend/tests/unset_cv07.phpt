--TEST--
unset() CV 7 (indirect unset() of global variable in import_request_variables())
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
