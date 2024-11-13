--TEST--
Test range() function with non finite numbers
--INI--
serialize_precision=14
--FILE--
<?php
// 0/0 NAN value
$f1 = fdiv(0, 0);
var_dump($f1);
// INF/INF NAN value
$f2 = fdiv(10.0**400, 9.0**400);
var_dump($f2);
// -INF + +INF NAN value
$f3 = (-10.0**400) + (9.0**400);
var_dump($f3);

$fs = [$f1, $f2, $f3, 5.5];

foreach ($fs as $s) {
    foreach ($fs as $e) {
        echo "range($s, $e);\n";
        try {
            var_dump( range($s, $e) );
        } catch (\ValueError $e) {
            echo $e->getMessage(), PHP_EOL;
        }
    }
}

?>
--EXPECT--
float(NAN)
float(NAN)
float(NAN)
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, 5.5);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, 5.5);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, NAN);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(NAN, 5.5);
range(): Argument #1 ($start) must be a finite number, NAN provided
range(5.5, NAN);
range(): Argument #2 ($end) must be a finite number, NAN provided
range(5.5, NAN);
range(): Argument #2 ($end) must be a finite number, NAN provided
range(5.5, NAN);
range(): Argument #2 ($end) must be a finite number, NAN provided
range(5.5, 5.5);
array(1) {
  [0]=>
  float(5.5)
}
