--TEST--
Packed zval sort and rsort preserve all comparison modes and stable value ordering
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
function check($actual, $expected) {
    if ($actual !== $expected) {
        throw new Exception(var_export([$actual, $expected], true));
    }
}
function verify($input, $flags) {
    foreach (['sort' => 'asort', 'rsort' => 'arsort'] as $sort => $referenceSort) {
        $original = serialize($input);
        $expected = $input;
        $referenceSort($expected, $flags);
        $values = $input;
        end($values);
        check($sort($values, $flags), true);
        check($values, array_values($expected));
        check(serialize($input), $original);
        check(key($values), 0);
        $values[] = 'appended';
        check(array_key_last($values), count($input));
    }
}
class SortableString {
    public function __construct(public string $value) {}
    public function __toString(): string { return $this->value; }
}
enum SortableEnum { case A; case B; }

setlocale(LC_COLLATE, 'C');
foreach ([2, 16, 17, 32, 63, 64, 65, 128, 1025] as $size) {
    $values = [];
    $strings = [];
    $objects = [];
    for ($i = 0; $i < $size; $i++) {
        $values[] = [1, 1.0, '1', '01', -0.0, 0, null, false, true, 2.5, '10', '2', INF, -INF][$i % 14];
        $strings[] = ['a10', 'A10', 'a2', 'A2', '1', '01'][$i % 6];
        $objects[] = new SortableString($strings[$i]);
    }
    $valuesWithHoles = $values;
    $stringsWithHoles = $strings;
    unset($valuesWithHoles[0], $stringsWithHoles[0]);
    foreach ([SORT_REGULAR, SORT_REGULAR | SORT_FLAG_CASE, 12345,
              SORT_NUMERIC, SORT_NUMERIC | SORT_FLAG_CASE,
              SORT_STRING, SORT_STRING | SORT_FLAG_CASE, SORT_NATURAL,
              SORT_NATURAL | SORT_FLAG_CASE, SORT_LOCALE_STRING,
              SORT_LOCALE_STRING | SORT_FLAG_CASE] as $flags) {
        verify($values, $flags);
        verify($strings, $flags);
        verify($valuesWithHoles, $flags);
        verify($stringsWithHoles, $flags);
        if (($flags & ~SORT_FLAG_CASE) !== SORT_NUMERIC) {
            verify($objects, $flags);
        }
    }
    echo "size $size: OK\n";
}

$values = [];
for ($i = 0; $i < 128; $i++) {
    $values[] = [SortableEnum::A, SortableEnum::B, null, 1, ['v' => 1], ['v' => 2]][$i % 6];
}
verify($values, SORT_REGULAR);
unset($values[1], $values[60], $values[127]);
verify($values, SORT_REGULAR);
echo "enums, nested arrays and holes: OK\n";

foreach (['sort' => 'asort', 'rsort' => 'arsort'] as $sort => $referenceSort) {
    $references = array_fill(0, 64, '01');
    $values = [];
    foreach ($references as &$value) {
        $values[] = &$value;
    }
    unset($value);
    $expected = $values;
    $referenceSort($expected, SORT_REGULAR);
    $sort($values, SORT_REGULAR);
    foreach ($references as $index => &$value) {
        $value = $index;
    }
    unset($value);
    check($values, array_values($expected));
    check($values, range(0, 63));
}
echo "stable references: OK\n";

$first = fopen('php://memory', 'r+');
$second = fopen('php://memory', 'r+');
$values = [];
for ($i = 0; $i < 64; $i++) $values[] = $i % 2 ? $first : $second;
foreach ([SORT_REGULAR, SORT_NUMERIC, SORT_STRING] as $flags) verify($values, $flags);
fclose($first);
fclose($second);
echo "resources: OK\n";
?>
--EXPECT--
size 2: OK
size 16: OK
size 17: OK
size 32: OK
size 63: OK
size 64: OK
size 65: OK
size 128: OK
size 1025: OK
enums, nested arrays and holes: OK
stable references: OK
resources: OK
