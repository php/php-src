--TEST--
Bug #73783: (SIG_IGN needs to be set to prevent syscals from returning early)
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
pcntl_signal(SIGCHLD, SIG_IGN);

switch(pcntl_fork()) {
    case 0:
        exit;
        break;
}

$before = microtime(true);
sleep(1);

if (microtime(true) - $before >= 0.8) {
    echo "working\n";
} else {
    echo "failed\n";
}
?>
--EXPECT--
working
