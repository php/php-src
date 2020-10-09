--TEST--
pcntl_alarm()
--SKIPIF--
<?php if (!function_exists("pcntl_sigtimedwait")) die("skip pcntl_sigtimedwait() not available"); ?>
--INI--
max_execution_time=0
--FILE--
<?php
pcntl_signal(SIGALRM, function(){});

pcntl_alarm(0);
var_dump(pcntl_alarm(60));
var_dump(pcntl_alarm(1) > 0);
$siginfo = array();
var_dump(pcntl_sigtimedwait(array(SIGALRM),$siginfo,2) === SIGALRM);
?>
--EXPECT--
int(0)
bool(true)
bool(true)
