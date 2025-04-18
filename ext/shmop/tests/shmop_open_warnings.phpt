--TEST--
shmop extension error messages
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--EXTENSIONS--
shmop
--FILE--
<?php

// Warning outputs: Unable to attach or create shared memory segment
var_dump(shmop_open(0, 'a', 0644, 1024));

// Shared memory segment size must be greater than zero
try {
    var_dump(shmop_open(0, 'a', 0644, 1024));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECTF--

Warning: shmop_open(): Unable to attach or create shared memory segment "%s" in %s on line %d
bool(false)

Warning: shmop_open(): Unable to attach or create shared memory segment "%s" in %s on line %d
bool(false)
