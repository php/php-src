--TEST--
zend_dval_to_lval preserves low bits  (64 bit long)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8)
     die("skip for machines with 64-bit longs");
?>
--FILE--
<?php
    /* test doubles around -4e21 */
    $values = [
        -4000000000000001048576.,
        -4000000000000000524288.,
        -4000000000000000000000.,
        -3999999999999999475712.,
        -3999999999999998951424.,
    ];

    foreach ($values as $v) {
        var_dump((int)$v);
    }

?>
--EXPECT--
int(2943463994971652096)
int(2943463994972176384)
int(2943463994972700672)
int(2943463994973224960)
int(2943463994973749248)
