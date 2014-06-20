--TEST--
pcntl_setaffinity()
--SKIPIF--
<?php 
if (!extension_loaded("pcntl")) print "skip"; 
if (!extension_loaded("posix")) print "skip"; 
?>
--FILE--
<?php
echo "ok. Please run 'taskset -pc ".posix_getpid()."'\n";
var_dump(pcntl_setaffinity(posix_getpid(), 0));
echo "Binding the cpu0\n";
sleep(10000);
?>
--EXPECT--
ok
