--TEST--
GH-23921 (Fibers start with error_reporting = 0 when the error_reporting INI directive is not set)
--FILE--
<?php
echo error_reporting(), "\n";
(new Fiber(function () {
    echo error_reporting(), "\n";
    echo $undefined;
    echo "after\n";
}))->start();
?>
--EXPECTF--
30719
30719

Warning: Undefined variable $undefined in %s on line 5
after
