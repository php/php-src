--TEST--
pcntl_daemonize()
--SKIPIF--
<?php 
if (!extension_loaded("pcntl")) print "skip"; 
?>
--FILE--
<?php
echo "ok. Processor will run in the background\n";
pcntl_daemonize();
sleep(10000);
echo "hello world\n";
?>
--EXPECT--
ok
