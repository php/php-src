--TEST--
Test array_filter() function : value cannot be converted to bool
--EXTENSIONS--
gmp
--FILE--
<?php

$input = [0, 1, -1, 10, 100, 1000];

function dump($value, $key)
{
  echo "$key = $value\n";
}

try {
    var_dump( array_filter($input, 'gmp_init') );
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$input = [gmp_init(5), gmp_init(25)];

try {
    var_dump( array_filter($input) );
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "Done"
?>
--EXPECT--
Object of class GMP could not be converted to bool
Object of class GMP could not be converted to bool
Done
