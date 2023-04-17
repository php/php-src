--TEST--
GH-10239 (proc_close after proc_get_status always returns -1)
--SKIPIF--
<?php
if (PHP_OS != "Linux") die("skip, only for linux");
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php
$p = proc_open('sleep 1', array(), $foo);
do {
    usleep(100000);
    $s = proc_get_status($p);
} while ($s['running']);
echo proc_close($p), PHP_EOL;
?>
--EXPECT--
0
