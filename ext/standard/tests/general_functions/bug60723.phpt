--TEST--
Bug #60723  (error_log error time has changed to UTC ignoring default timezo)
--INI--
date.timezone=ASIA/Chongqing
log_errors=On
--FILE--
<?php
$dir = __DIR__;
$log = $dir . "/tmp.err";
ini_set("error_log", $log);
echo $aa;
error_log("dummy");
readfile($log);
unlink($log);
?>
--EXPECTF--
Warning: Undefined variable $aa in %s on line %d
[%s ASIA/Chongqing] PHP Warning:  Undefined variable $aa in %s on line %d
[%s ASIA/Chongqing] dummy
