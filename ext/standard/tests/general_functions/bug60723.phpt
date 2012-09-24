--TEST--
Bug #55371 (get_magic_quotes_gpc() and get_magic_quotes_runtime() throw deprecated warning)
--INI--
date.timezone=ASIA/Chongqing
log_errors=On
--FILE--
<?php
$dir = dirname(__FILE__);
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
