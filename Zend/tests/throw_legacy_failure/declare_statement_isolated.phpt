--TEST--
Test throw_legacy_failure declare statement is isolated to file
--FILE--
<?php
include 'fixture/no_declare.inc';
try {
    include 'fixture/declare_on.inc';
} catch (\Throwable $e) {
    echo "Error caught\n";
}
include 'fixture/declare_off.inc';
?>
DONE
--EXPECTF--
Warning: array_combine(): Both parameters should have an equal number of elements in %sfixture/no_declare.inc on line %d
Error caught

Warning: array_combine(): Both parameters should have an equal number of elements in %sfixture/declare_off.inc on line %d
DONE
