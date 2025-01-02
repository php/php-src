--TEST--
gmp_cmp() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

function cmp_helper($l, $r) {
    echo 'gmp(', var_export($l, true), ', ', var_export($r, true), '): ';
    $r = gmp_cmp($l, $r);
    echo match (true) {
        $r === 0 => "equals\n",
        $r < 0 => "right greater than left\n",
        $r > 0 => "left greater than right\n",
    };
}

cmp_helper(123123,-123123);
cmp_helper("12345678900987654321","12345678900987654321");
cmp_helper("12345678900987654321","123456789009876543211");
cmp_helper(0,0);
cmp_helper(1231222,0);
cmp_helper(0,345355);

$n = gmp_init("827278512385463739");
var_dump(gmp_cmp(0,$n) < 0);
$n1 = gmp_init("827278512385463739");
var_dump(gmp_cmp($n1,$n) === 0);

try {
    var_dump(gmp_cmp(array(),array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
gmp(123123, -123123): left greater than right
gmp('12345678900987654321', '12345678900987654321'): equals
gmp('12345678900987654321', '123456789009876543211'): right greater than left
gmp(0, 0): equals
gmp(1231222, 0): left greater than right
gmp(0, 345355): right greater than left
bool(true)
bool(true)
gmp_cmp(): Argument #1 ($num1) must be of type GMP|string|int, array given
Done
