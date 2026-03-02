--TEST--
Bug #35014 (array_product() always returns 0) (32bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php
$tests = array(
    array(0),
    array(3),
    array(3, 3),
    array(0.5, 2),
    array(99999999, 99999999),
    array(8.993, 7443241,988, sprintf("%u", -1)+0.44),
    array(2,sprintf("%u", -1)),
);

foreach ($tests as $v) {
    var_dump(array_product($v));
}
?>
--EXPECT--
int(0)
int(3)
int(9)
float(1)
float(9999999800000000)
float(2.8404260053902914E+20)
float(8589934590)
