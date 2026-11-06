--TEST--
GH-24563: excessive precision preserves exact binary64 formatting
--FILE--
<?php
function formatValues(array $values): array {
    $result = [];
    foreach ($values as $value) {
        ob_start();
        var_dump($value);
        $dump = ob_get_clean();
        $result[] = [var_export($value, true), serialize($value),
            json_encode($value), $dump, (string) $value];
    }
    return $result;
}
$smallestSubnormal = unpack('E', hex2bin('0000000000000001'))[1];
$largestSubnormal = unpack('E', hex2bin('000fffffffffffff'))[1];
$values = [0.1, PHP_FLOAT_MAX, PHP_FLOAT_MIN, $smallestSubnormal,
    $largestSubnormal, -$largestSubnormal, -0.0, 0.0, INF, -INF, NAN,
    1.2345678901234567, 0.0001, 0.00001, 1e17, 1e300];
ini_set('precision', '770');
ini_set('serialize_precision', '770');
$expected = formatValues($values);
$digits = explode('E', var_export($largestSubnormal, true))[0];
echo 'Largest subnormal significant digits: ', strlen(str_replace('.', '', $digits)), "\n";
foreach ([771, 1000, 4194304] as $precision) {
    ini_set('precision', (string) $precision);
    ini_set('serialize_precision', (string) $precision);
    echo $precision, ': ', formatValues($values) === $expected ? 'identical' : 'different', "\n";
}
echo var_export(0.1, true), "\n";
ini_set('precision', '-1');
ini_set('serialize_precision', '-1');
echo var_export(0.1, true), ' ', (string) 0.1, "\n";
?>
--EXPECT--
Largest subnormal significant digits: 767
771: identical
1000: identical
4194304: identical
0.1000000000000000055511151231257827021181583404541015625
0.1 0.1
