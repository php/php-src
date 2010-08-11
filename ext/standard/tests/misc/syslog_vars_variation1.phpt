--TEST--
define_syslog_variables() variation 1
--INI--
define_syslog_variables=On
--FILE--
<?php
var_dump(isset($LOG_ERR));
?>
--EXPECTF--
Deprecated: Directive 'define_syslog_variables' is deprecated in PHP 5.3 and greater in Unknown on line 0
bool(true)
