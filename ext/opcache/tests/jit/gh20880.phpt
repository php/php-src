--TEST--
GH-20880 (JIT (tracing): NAN float comparisons incorrectly return true)
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function observe(float $value, float $max): float {
    if ($value > $max) {
        return $value;
    }
    return $max;
}


$max = 0.0;
for ($i = 0; $i < 100000; $i++) {
    $max = observe(1.0, $max);
    $max = observe(3.0, $max);
}

$max = observe(4.0, $max);
$max = observe(NAN, $max);
var_dump($max);
?>
--EXPECT--
float(4)
