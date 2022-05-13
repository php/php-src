--TEST--
Test pcntl_get_last_error()
--EXTENSIONS--
pcntl
--FILE--
<?php
var_dump(pcntl_get_last_error());
$pid = pcntl_wait($status);
var_dump($pid);
var_dump(pcntl_get_last_error() == PCNTL_ECHILD);
?>
--EXPECT--
int(0)
int(-1)
bool(true)
