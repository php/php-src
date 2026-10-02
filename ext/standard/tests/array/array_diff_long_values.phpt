--TEST--
array_diff() with long values
--FILE--
<?php
function dump_array(array $value): void {
    echo json_encode($value), "\n";
}

dump_array(array_diff([1, 2, 3], [2]));
dump_array(array_diff([-2, -1, 0, 1], [-1, 1]));
dump_array(array_diff([PHP_INT_MIN, -1, PHP_INT_MAX], [PHP_INT_MIN, PHP_INT_MAX]));
var_dump(array_values(array_diff(
    [PHP_INT_MIN, PHP_INT_MIN + 1, PHP_INT_MIN + 2, PHP_INT_MIN + 3],
    [PHP_INT_MIN, PHP_INT_MIN + 2, PHP_INT_MIN + 2],
)) === [PHP_INT_MIN + 1, PHP_INT_MIN + 3]);
var_dump(array_values(array_diff(
    [PHP_INT_MAX - 3, PHP_INT_MAX - 2, PHP_INT_MAX - 1, PHP_INT_MAX],
    [PHP_INT_MAX - 2, PHP_INT_MAX, PHP_INT_MAX],
)) === [PHP_INT_MAX - 3, PHP_INT_MAX - 1]);
var_dump(array_diff(
    [PHP_INT_MIN, 10, 11, 12, PHP_INT_MAX],
    [11],
) === [PHP_INT_MIN, 10, 3 => 12, PHP_INT_MAX]);
dump_array(array_diff([1, 2, 2, 3], [2]));
dump_array(array_diff(['a' => 1, 'b' => 2, 'c' => 3], [2]));
dump_array(array_diff([1, 2, 3], ['2']));
dump_array(array_diff([1, '2', 3], [2]));
dump_array(array_diff([1, 2, 3, 4], [2], [4, 5]));
dump_array(array_diff([1, 2, 3, 4], [2], ['3']));
dump_array(array_diff([1, 2], [1, 2]));

$y = 2;
dump_array(array_diff([1, 2, 3], [&$y]));

$x = 1;
$array = [&$x, 2];
$result = array_diff($array, [2]);
$x = 9;
var_dump($result[0]);

$sparseValues = [];
for ($i = 1; $i <= 256; $i++) {
    $sparseValues[] = $i * 65536;
}
var_dump(array_diff([1, 2], $sparseValues) === [1, 2]);
?>
--EXPECT--
{"0":1,"2":3}
{"0":-2,"2":0}
{"1":-1}
bool(true)
bool(true)
bool(true)
{"0":1,"3":3}
{"a":1,"c":3}
{"0":1,"2":3}
{"0":1,"2":3}
{"0":1,"2":3}
{"0":1,"3":4}
[]
{"0":1,"2":3}
int(9)
bool(true)
