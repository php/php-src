--TEST--
Bug #44827 (Class error when trying to access :: as constant)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest Munich 2009
--FILE--
<?php
try {
    define('::', true);
}
catch (Error $ex) {
    echo '[' . get_type($ex) . '] ' . $ex->getMessage() . "\n");
}
catch (\Throwable $ex) {
   die('ERROR = ' . $ex);
}

?>
--EXPECTF--
Warning: Class constants cannot be defined or redefined in %s on line %d

Fatal error: Class '' not found in %s on line %d
