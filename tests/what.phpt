--TEST--
chmod(0444) makes a file read-only
--FILE--
<?php
$filename = __DIR__ . "/what.txt";
var_dump(touch($filename));
var_dump(chmod($filename, 0444));
clearstatcache();
var_dump(decoct(fileperms($filename)));
var_dump(file_put_contents($filename, "foo"));
var_dump(file_get_contents($filename));
var_dump(file_get_contents("/etc/rc.conf"));
?>
--CLEAN--
<?php
@chmod(__DIR__ . "/what.txt", 0644);
@unlink(__DIR__ . "/what.txt");
?>
--EXPECTF--
bool(true)
bool(true)
string(6) "100444"

Warning: file_put_contents(%swhat.txt): Failed to open stream: Permission denied in %s on line %d
bool(false)
string(0) ""
