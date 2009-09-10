--TEST--
Check the php_ini_loaded_file() function. No file is loaded in test, so false ins returned
--CREDITS--
Sebastian Schürmann
sschuermann@chip.de
Testfest 2009 Munich
--FILE--
<?php
var_dump(php_ini_loaded_file());
?>
--EXPECTF--
string(%d) "%s/tmp-php.ini"
