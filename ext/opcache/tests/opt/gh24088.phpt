--TEST--
GH-24088 (Integer casts must not reuse ranges inferred for other types)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function bounded($value) {
    if ($value > 5 && $value < 7) {
        return match ((int) $value) {
            6 => 'six',
            default => 'not six',
        };
    }
}

function equal($value) {
    if ($value == 6) {
        return (int) $value;
    }
}

function copied($value, $copy) {
    if ($value == 6) {
        if ($copy) {
            $result = $value;
        } else {
            $result = 6;
        }
        return (int) $result;
    }
}

echo 'float: ', bounded(5.5), "\n";
echo 'numeric string: ', bounded('5.5'), "\n";
echo 'integer: ', bounded(6), "\n";
echo 'boolean: ', equal(true), "\n";
echo 'copied boolean: ', copied(true, true), "\n";
echo 'joined integer: ', copied(true, false), "\n";
?>
--EXPECT--
float: not six
numeric string: not six
integer: six
boolean: 1
copied boolean: 1
joined integer: 6
