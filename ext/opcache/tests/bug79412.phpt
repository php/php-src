--TEST--
Bug #79412 (Opcache chokes and uses 100% CPU on specific script)
--INI--
opcache.enable=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$limitPerRun = 10;
foreach ($foo as $bar) {
    $count = 0;
    try {
        foreach ($runs as $run) {
            ++$count;
            if ($count >= $limitPerRun) {
                break;
            }
        }
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
    foo($limitPerRun);
}
?>
--EXPECTF--
Warning: Undefined variable $foo in %s on line %d
foreach() argument must be of type array|object, null given
