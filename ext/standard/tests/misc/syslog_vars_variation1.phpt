--TEST--
define_syslog_variables() variation 1
--INI--
define_syslog_variables=On
--FILE--
<?php
var_dump(isset($LOG_ERR));
?>
--EXPECTF--
bool(true)
