--TEST--
pcntl_signal() returns false for SIGKILL and SIGSTOP
--EXTENSIONS--
pcntl
--FILE--
<?php
foreach ([SIGKILL, SIGSTOP] as $signo) {
    var_dump(pcntl_signal($signo, SIG_IGN));
    var_dump(pcntl_signal($signo, SIG_DFL));
    var_dump(pcntl_signal($signo, function () {}));
    var_dump(pcntl_get_last_error() === PCNTL_EINVAL);
}
echo "alive\n";
?>
--EXPECTF--
Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)
bool(true)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)

Warning: pcntl_signal(): Error assigning signal in %s on line %d
bool(false)
bool(true)
alive
