--TEST--
Bug #69803: gmp_random_range() modifies second parameter if GMP number
--EXTENSIONS--
gmp
--FILE--
<?php

$a = gmp_init(100);
$b = gmp_init(200);
echo $a . ", ", $b . "\n";
gmp_random_range($a, $b);
echo $a . ", ", $b . "\n";

$b = gmp_init(200);
echo $a . ", ", $b . "\n";
gmp_random_range(100, $b);
echo $a . ", ", $b . "\n";

?>
--EXPECT--
100, 200
100, 200
100, 200
100, 200
