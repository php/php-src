--TEST--
error_reporting via INI: Konstanten-Ausdruck (E_ERROR | E_CORE_ERROR)
--ENV--
PHP_INI_ERROR_REPORTING="E_ERROR | E_CORE_ERROR"
--INI--
error_reporting="${PHP_INI_ERROR_REPORTING}"
--FILE--
<?php
echo "error_reporting: ", error_reporting(), "\n";
echo "E_ERROR | E_CORE_ERROR: ", (E_ERROR | E_CORE_ERROR), "\n";
--EXPECTF--
error_reporting: %d
E_ERROR | E_CORE_ERROR: %d
