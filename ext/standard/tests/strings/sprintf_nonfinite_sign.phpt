--TEST--
sprintf sign handling for finite and non-finite floating-point values
--FILE--
<?php
$negative_nan = unpack('E', hex2bin('fff8000000000000'))[1];
$values = [
    'finite positive' => 1.5,
    'finite negative' => -1.5,
    'positive infinity' => INF,
    'negative infinity' => -INF,
    'positive NaN' => NAN,
    'negative NaN' => $negative_nan,
];
$formats = ['%f', '%+f', '%10f', '%+10f', '%.2f', '%+.2f', '%010f', '%+010f'];
foreach ($values as $name => $value) {
    echo $name, "\n";
    foreach ($formats as $format) {
        echo $format, '=[', sprintf($format, $value), "]\n";
    }
}

$cases = [
    [INF, ['INF', '+INF', '       INF', '      +INF', '0000000INF', '+000000INF', 'INF       ', '+INF      ', '_______INF', '______+INF']],
    [-INF, ['-INF', '-INF', '      -INF', '      -INF', '-000000INF', '-000000INF', '-INF      ', '-INF      ', '______-INF', '______-INF']],
    [NAN, ['NaN', '+NaN', '       NaN', '      +NaN', '0000000NaN', '+000000NaN', 'NaN       ', '+NaN      ', '_______NaN', '______+NaN']],
    [$negative_nan, ['NaN', '+NaN', '       NaN', '      +NaN', '0000000NaN', '+000000NaN', 'NaN       ', '+NaN      ', '_______NaN', '______+NaN']],
    [pow(-1.0, 0.3), ['NaN', '+NaN', '       NaN', '      +NaN', '0000000NaN', '+000000NaN', 'NaN       ', '+NaN      ', '_______NaN', '______+NaN']],
];
$flags = ['', '+', '10', '+10', '010', '+010', '-10', '+-10', "'_10", "+'_10"];
foreach ($cases as [$value, $expected]) {
    foreach (str_split('eEfFgGhH') as $specifier) {
        foreach ($flags as $i => $flag) {
            foreach (['', '.0', '.2'] as $precision) {
                $format = '%' . $flag . $precision . $specifier;
                $actual = sprintf($format, $value);
                if ($actual !== $expected[$i]) {
                    var_dump($format, $actual, $expected[$i]);
                }
            }
        }
    }
}
echo "All conversion specifiers checked\n";
?>
--EXPECT--
finite positive
%f=[1.500000]
%+f=[+1.500000]
%10f=[  1.500000]
%+10f=[ +1.500000]
%.2f=[1.50]
%+.2f=[+1.50]
%010f=[001.500000]
%+010f=[+01.500000]
finite negative
%f=[-1.500000]
%+f=[-1.500000]
%10f=[ -1.500000]
%+10f=[ -1.500000]
%.2f=[-1.50]
%+.2f=[-1.50]
%010f=[-01.500000]
%+010f=[-01.500000]
positive infinity
%f=[INF]
%+f=[+INF]
%10f=[       INF]
%+10f=[      +INF]
%.2f=[INF]
%+.2f=[+INF]
%010f=[0000000INF]
%+010f=[+000000INF]
negative infinity
%f=[-INF]
%+f=[-INF]
%10f=[      -INF]
%+10f=[      -INF]
%.2f=[-INF]
%+.2f=[-INF]
%010f=[-000000INF]
%+010f=[-000000INF]
positive NaN
%f=[NaN]
%+f=[+NaN]
%10f=[       NaN]
%+10f=[      +NaN]
%.2f=[NaN]
%+.2f=[+NaN]
%010f=[0000000NaN]
%+010f=[+000000NaN]
negative NaN
%f=[NaN]
%+f=[+NaN]
%10f=[       NaN]
%+10f=[      +NaN]
%.2f=[NaN]
%+.2f=[+NaN]
%010f=[0000000NaN]
%+010f=[+000000NaN]
All conversion specifiers checked
