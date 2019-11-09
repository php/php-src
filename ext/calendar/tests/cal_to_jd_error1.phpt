--TEST--
Test cal_to_jd() function : error conditions
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
try {
    cal_to_jd(-1, 8, 26, 74);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
invalid calendar ID -1
