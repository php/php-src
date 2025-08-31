--TEST--
Bug #79412 (Opcache chokes and uses 100% CPU on specific script)
--INI--
opcache.enable=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
$limitPerRun = 10;
foreach ($foo as $bar) {
    $count = 0;
    foreach ($runs as $run) {
        ++$count;
        if ($count >= $limitPerRun) {
            break;
        }
    }
    foo($limitPerRun);
}
?>
--EXPECTF--
Warning: Undefined variable $foo in %s on line %d

Warning: foreach() argument must be of type array|object, null given in %s on line %d
