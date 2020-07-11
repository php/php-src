--TEST--
bug 7715, floats value with integer or incomplete input
--INI--
precision=14
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$data = array(
    '.23',
    '-42',
    '+42',
    '.4',
    '-.4',
    '1000000000000',
    '-1000000000000',
    '02.324'
);
foreach ($data as $val) {
    $res = filter_var($val, FILTER_VALIDATE_FLOAT);
    var_dump($res);
}
echo "\n";
?>
--EXPECT--
float(0.23)
float(-42)
float(42)
float(0.4)
float(-0.4)
float(1000000000000)
float(-1000000000000)
float(2.324)
