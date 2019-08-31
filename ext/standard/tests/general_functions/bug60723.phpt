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
Notice: Undefined variable: aa in %sbug60723.php on line %d
[%s ASIA/Chongqing] PHP Notice:  Undefined variable: aa in %sbug60723.php on line %d
[%s ASIA/Chongqing] dummy
