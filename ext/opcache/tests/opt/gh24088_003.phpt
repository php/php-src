--TEST--
GH-24088 (Symbolic ranges must account for the compared operand type)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function symbolic($value, int $integer) {
    if ($value == 6 && $integer == $value) {
        return $integer;
    }
}

function symbolicBounds($value, int $integer) {
    if ($value == 6 && $integer == $value) {
        return $integer < 3 ? 'small' : ($integer > 9 ? 'large' : 'medium');
    }
}

function loop() {
    $sum = 0;
    for ($i = 0; $i < 10; $i++) {
        $sum += $i;
    }
    return $sum;
}

echo 'symbolic boolean: ', symbolic(true, 42), "\n";
echo 'symbolic integer: ', symbolic(6, 6), "\n";
echo 'symbolic lower bound: ', symbolicBounds(true, 1), "\n";
echo 'symbolic upper bound: ', symbolicBounds(true, 42), "\n";
echo 'integer loop: ', loop(), "\n";
?>
--EXPECT--
symbolic boolean: 42
symbolic integer: 6
symbolic lower bound: small
symbolic upper bound: large
integer loop: 45
