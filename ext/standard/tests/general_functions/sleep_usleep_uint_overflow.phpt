--TEST--
sleep() and usleep() reject values above their unsigned int limits
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die('skip 64-bit only');
?>
--FILE--
<?php
var_dump(sleep(0));
usleep(0);
echo "usleep(0) ok\n";

foreach (['sleep', 'usleep'] as $function) {
    try {
        $function(4294967296);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
int(0)
usleep(0) ok
ValueError: sleep(): Argument #1 ($seconds) must be between 0 and %d
ValueError: usleep(): Argument #1 ($microseconds) must be between 0 and 4294967295
