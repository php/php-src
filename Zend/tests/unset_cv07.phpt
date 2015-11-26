--TEST--
unset() CV 7 (indirect unset() of global variable in import_request_variables())
--SKIPIF--
<?php if(PHP_VERSION_ID >= 50399){ die('skip not needed anymore without register_globals'); } ?>
--GET--
x=2
--FILE--
<?php
$_x = "1\n";
echo $_x;
import_request_variables("g","_");
echo $_x;
echo "\nok\n";
?>
--EXPECTF--
1
2
ok
