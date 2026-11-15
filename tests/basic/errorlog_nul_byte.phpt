--TEST--
NUL bytes in error messages are escaped in the error log
--INI--
log_errors=1
display_errors=0
error_log={PWD}/errorlog_nul_byte.log
--FILE--
<?php
$o = new class {};
$o->dyn = 1;
trigger_error("foo\0bar", E_USER_WARNING);
echo file_get_contents(__DIR__ . '/errorlog_nul_byte.log');
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/errorlog_nul_byte.log');
?>
--EXPECTF--
[%s] PHP Deprecated:  Creation of dynamic property class@anonymous\0%s::$dyn is deprecated in %s on line %d
[%s] PHP Warning:  foo\0bar in %s on line %d
