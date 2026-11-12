--TEST--
GH-25388 (Invalid access or leak of ext/date tz cache after logging during startup)
--CREDITS--
bwoebi
--INI--
allow_url_include=1
log_errors=1
error_log={PWD}/gh25388.log
output_handler=inexistent
date.timezone=Europe/Berlin
--FILE--
<?php
echo date('e'), "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25388.log');
?>
--EXPECTF--
Deprecated: Directive 'allow_url_include' is deprecated in Unknown on line 0

Warning: PHP Request Startup: function "inexistent" not found or invalid function name in Unknown on line 0
Europe/Berlin
