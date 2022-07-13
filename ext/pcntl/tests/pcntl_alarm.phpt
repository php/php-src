--TEST--
pcntl_alarm()
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
    if (!function_exists("pcntl_sigtimedwait")) die("skip pcntl_sigtimedwait() not available");
    if (!function_exists("pcntl_alarm")) die("skip pcntl_alarm() not available");
    if (!function_exists("pcntl_signal")) die("skip pcntl_signal() not available");
?>
--INI--
max_execution_time=3
--FILE--
<?php
pcntl_signal(SIGALRM, function(){});

var_dump(pcntl_alarm(0));
var_dump(pcntl_alarm(60));
var_dump(pcntl_alarm(1) > 0);
$siginfo = array();
var_dump(pcntl_sigtimedwait(array(SIGALRM),$siginfo,1) === SIGALRM);
?>
--EXPECT--
int(0)
int(0)
bool(true)
bool(true)
