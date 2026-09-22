--TEST--
GH-22549 (Assertion failure in _zendi_try_convert_scalar_to_number with compound GMP shift/pow)
--EXTENSIONS--
gmp
--FILE--
<?php
// Compound assignment aliases result and op1. A negative GMP shift/exponent
// must throw a ValueError, not clobber op1 into IS_UNDEF and abort.
$cases = [
    'int ** gmp'   => [2,            fn(&$x) => $x **= gmp_init(-5)],
    'gmp ** gmp'   => [gmp_init(2),  fn(&$x) => $x **= gmp_init(-5)],
    'array ** gmp' => [[0],          fn(&$x) => $x **= gmp_init(-5)],
    'int << gmp'   => [8,            fn(&$x) => $x <<= gmp_init(-2)],
    'int >> gmp'   => [8,            fn(&$x) => $x >>= gmp_init(-2)],
    'gmp << gmp'   => [gmp_init(8),  fn(&$x) => $x <<= gmp_init(-2)],
];

foreach ($cases as $label => [$x, $op]) {
    try {
        $op($x);
        echo "$label: no exception\n";
    } catch (\Throwable $e) {
        echo "$label: ", $e::class, ": ", $e->getMessage(), "\n";
    }
    var_dump($x);
}

// Valid operations must still succeed (guard against an over-broad regression).
$p = 2;   $p **= gmp_init(3);  echo "valid pow: ", gmp_strval($p), "\n";
$s = 1;   $s <<= gmp_init(4);  echo "valid shift: ", gmp_strval($s), "\n";
?>
--EXPECTF--
int ** gmp: ValueError: Exponent must be between 0 and %d
int(2)
gmp ** gmp: ValueError: Exponent must be between 0 and %d
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "2"
}
array ** gmp: ValueError: Exponent must be between 0 and %d
array(1) {
  [0]=>
  int(0)
}
int << gmp: ValueError: Shift must be between 0 and %d
int(8)
int >> gmp: ValueError: Shift must be between 0 and %d
int(8)
gmp << gmp: ValueError: Shift must be between 0 and %d
object(GMP)#%d (1) {
  ["num"]=>
  string(1) "8"
}
valid pow: 8
valid shift: 16
