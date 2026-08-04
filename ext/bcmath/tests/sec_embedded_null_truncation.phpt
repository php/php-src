--TEST--
bcmath strings with embedded null bytes must be rejected as not well-formed
--EXTENSIONS--
bcmath
--FILE--
<?php
$cases = [
    "100\x005",
    "1.2\x003",
    "\x00123",
];

$calls = [
    fn($s) => bcadd($s, "0"),
    fn($s) => bcsub($s, "0"),
    fn($s) => bcmul($s, "1"),
    fn($s) => bcdiv($s, "1"),
    fn($s) => bcmod($s, "7"),
    fn($s) => bcpow($s, "1"),
    fn($s) => bccomp($s, "0"),
    fn($s) => bcsqrt($s),
];

foreach ($cases as $s) {
    foreach ($calls as $fn) {
        try {
            $fn($s);
            echo "FAIL: accepted\n";
        } catch (\ValueError $e) {
            echo $e->getMessage() . "\n";
        }
    }
}
?>
--EXPECT--
bcadd(): Argument #1 ($num1) is not well-formed
bcsub(): Argument #1 ($num1) is not well-formed
bcmul(): Argument #1 ($num1) is not well-formed
bcdiv(): Argument #1 ($num1) is not well-formed
bcmod(): Argument #1 ($num1) is not well-formed
bcpow(): Argument #1 ($num) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
bcsqrt(): Argument #1 ($num) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed
bcsub(): Argument #1 ($num1) is not well-formed
bcmul(): Argument #1 ($num1) is not well-formed
bcdiv(): Argument #1 ($num1) is not well-formed
bcmod(): Argument #1 ($num1) is not well-formed
bcpow(): Argument #1 ($num) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
bcsqrt(): Argument #1 ($num) is not well-formed
bcadd(): Argument #1 ($num1) is not well-formed
bcsub(): Argument #1 ($num1) is not well-formed
bcmul(): Argument #1 ($num1) is not well-formed
bcdiv(): Argument #1 ($num1) is not well-formed
bcmod(): Argument #1 ($num1) is not well-formed
bcpow(): Argument #1 ($num) is not well-formed
bccomp(): Argument #1 ($num1) is not well-formed
bcsqrt(): Argument #1 ($num) is not well-formed
