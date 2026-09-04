--TEST--
pack() accepts GMP values for float and double format codes
--EXTENSIONS--
gmp
--FILE--
<?php

$value = gmp_init(42);
foreach (['f', 'g', 'G', 'd', 'e', 'E'] as $format) {
    echo "$format: ";
    var_dump(unpack($format, pack($format, $value))[1]);
}

?>
--EXPECT--
f: float(42)
g: float(42)
G: float(42)
d: float(42)
e: float(42)
E: float(42)
