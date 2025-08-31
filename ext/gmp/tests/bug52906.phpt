--TEST--
Bug #52906 gmp_mod returns negative result when non-negative is expected
--EXTENSIONS--
gmp
--FILE--
<?php

$vals = array(
    array(7, 3),
    array(2, 7),
    array(12, 7),
    array(-2, 7),
    array(-12, 7),
    array(2, -7),
    array(12, -7),
    array(-2, -7),
    array(-12, -7),
);
foreach($vals as $data) {
    echo "{$data[0]}%{$data[1]}=".gmp_strval(gmp_mod($data[0], $data[1]));
    echo "\n";
    echo "{$data[0]}%{$data[1]}=".gmp_strval(gmp_mod(gmp_init($data[0]), gmp_init($data[1])));
    echo "\n";
}
echo "Done\n";
?>
--EXPECT--
7%3=1
7%3=1
2%7=2
2%7=2
12%7=5
12%7=5
-2%7=5
-2%7=5
-12%7=2
-12%7=2
2%-7=2
2%-7=2
12%-7=5
12%-7=5
-2%-7=5
-2%-7=5
-12%-7=2
-12%-7=2
Done
