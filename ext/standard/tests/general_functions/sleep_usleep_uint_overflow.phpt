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
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECTF--
int(0)
usleep(0) ok
sleep(): Argument #1 ($seconds) must be less than or equal to %d
usleep(): Argument #1 ($microseconds) must be less than or equal to 4294967295
