--TEST--
SplFileObject::setMaxLineLen() $maxLength overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
/* setMaxLineLen() stores the argument in a size_t field after checking only
 * that it is not negative, so a length above SIZE_MAX is silently narrowed:
 * getMaxLineLen() reports a different number than was set, and the value later
 * reaches safe_emalloc() as max_line_len + 1, which wraps to 0 when the
 * narrowing produced SIZE_MAX. */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;
$file = new SplFileObject(__FILE__);

/* Lengths that fit size_t are kept exactly. */
foreach (['0' => 0, '1' => 1, 'SIZE_MAX' => $sizeMax] as $label => $length) {
    $file->setMaxLineLen($length);
    printf("%s: kept %s\n", $label, var_export($file->getMaxLineLen() === $length, true));
}

/* Lengths that do not fit have to be refused, leaving the previous value. */
$lengths = [
    'SIZE_MAX+1'  => $sizeMax + 1,
    'SIZE_MAX+11' => $sizeMax + 11,
    'INT_MAX'     => PHP_INT_MAX,
];

foreach ($lengths as $label => $length) {
    echo "$label: ";
    try {
        $file->setMaxLineLen($length);
        echo "unexpected success; getMaxLineLen() = ", $file->getMaxLineLen(), "\n";
    } catch (Error $e) {
        echo $e::class . ': ' . $e->getMessage(), "\n";
    }
}

printf("unchanged after the refusals: %s\n",
    var_export($file->getMaxLineLen() === $sizeMax, true));
?>
--EXPECTF--
0: kept true
1: kept true
SIZE_MAX: kept true
SIZE_MAX+1: ValueError: SplFileObject::setMaxLineLen(): Argument #1 ($maxLength) must be less than or equal to %d
SIZE_MAX+11: ValueError: SplFileObject::setMaxLineLen(): Argument #1 ($maxLength) must be less than or equal to %d
INT_MAX: ValueError: SplFileObject::setMaxLineLen(): Argument #1 ($maxLength) must be less than or equal to %d
unchanged after the refusals: true
