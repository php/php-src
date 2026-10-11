--TEST--
GH-24088 (Range propagation must account for implicit integer conversions)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
error_reporting=E_ALL & ~E_DEPRECATED & ~E_WARNING
--FILE--
<?php
function operations($value) {
    if ($value == 6) {
        return [
            'modulo' => $value % 100,
            'left shift' => $value << 0,
            'right shift' => $value >> 0,
            'bitwise or' => $value | 0,
            'bitwise and' => $value & 255,
            'addition' => (int) ($value + 0),
            'subtraction' => (int) ($value - 0),
            'multiplication' => (int) ($value * 1),
            'division' => (int) ($value / 1),
        ];
    }
}

function bitwiseNot($value) {
    if ($value > 5 && $value < 7) {
        $result = ~$value;
        if (is_int($result)) {
            return $result;
        }
    }
}

function assignment($value) {
    if ($value == 6) {
        $value %= 100;
        return $value;
    }
}

function increment($value) {
    if ($value == 6) {
        return (int) ++$value;
    }
}

function decrement($value) {
    if ($value == 6) {
        return (int) --$value;
    }
}

function postIncrement($value) {
    if ($value == 6) {
        $result = $value++;
        return [(int) $result, (int) $value];
    }
}

function postDecrement($value) {
    if ($value == 6) {
        $result = $value--;
        return [(int) $result, (int) $value];
    }
}

foreach (operations(true) as $operation => $result) {
    echo $operation, ': ', $result, "\n";
}
echo 'bitwise not: ', bitwiseNot(5.5), "\n";
echo 'assignment: ', assignment(true), "\n";
echo 'pre-increment: ', increment(true), "\n";
echo 'pre-decrement: ', decrement(true), "\n";
echo 'post-increment: ', implode(', ', postIncrement(true)), "\n";
echo 'post-decrement: ', implode(', ', postDecrement(true)), "\n";
?>
--EXPECT--
modulo: 1
left shift: 1
right shift: 1
bitwise or: 1
bitwise and: 1
addition: 1
subtraction: 1
multiplication: 1
division: 1
bitwise not: -6
assignment: 1
pre-increment: 1
pre-decrement: 1
post-increment: 1, 1
post-decrement: 1, 1
