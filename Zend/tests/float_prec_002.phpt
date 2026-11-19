--TEST--
Float multiplication and division with subnormal results are correctly rounded
--DESCRIPTION--
The x87 FPU rounds such results twice (into its wide exponent range, then when
storing the double), which can leave them one ULP away from the IEEE-754
binary64 result.
--FILE--
<?php
$cases = [
    [5.678650331314532e-158, '*', 2.3715405351663569e-153],
    [3.845033612074171e-157, '/', 2.9261099430959845e+151],
    [1.6719640777648563e-157, '/', 4.8865591888918705e+151],
];
foreach ($cases as [$a, $op, $b]) {
    $r = $op === '*' ? $a * $b : $a / $b;
    var_dump($r, bin2hex(pack('E', $r)));
}
?>
--EXPECT--
float(1.3467149445748E-310)
string(16) "000018ca74512179"
float(1.3140427690170504E-308)
string(16) "000972f0870d58e1"
float(3.42155699569866E-309)
string(16) "000275da670e8e41"
