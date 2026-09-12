--TEST--
fread() $length overflow on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$path = sys_get_temp_dir() . '/fread_length_sizet.tmp';
file_put_contents($path, str_repeat('A', 65536));
$f = fopen($path, 'r');

try {
    fread($f, PHP_INT_MAX);
    echo "unexpected success\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

/* fread() reaches zend_string_alloc($length) via php_stream_read_to_str().
 * zend_string_alloc() adds the header and the terminating NUL to the length
 * without an overflow check, so anything above PHP_STRING_MAX_LENGTH wraps to
 * a tiny allocation that the following read then overruns. The whole window
 * from the limit up to SIZE_MAX has to be refused, not just the values that
 * exceed SIZE_MAX itself. */
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;

$lengths = [
    'STR_MAX+1'  => PHP_STRING_MAX_LENGTH + 1,
    'STR_MAX+2'  => PHP_STRING_MAX_LENGTH + 2,
    'STR_MAX+3'  => PHP_STRING_MAX_LENGTH + 3,
    'SIZE_MAX-2' => $sizeMax - 2,
    'SIZE_MAX-1' => $sizeMax - 1,
    'SIZE_MAX'   => $sizeMax,
];

foreach ($lengths as $label => $length) {
    try {
        fread($f, $length);
        echo "$label: unexpected success\n";
    } catch (ValueError $e) {
        echo "$label: {$e->getMessage()}\n";
    }
}

fclose($f);
?>
--CLEAN--
<?php
@unlink(sys_get_temp_dir() . '/fread_length_sizet.tmp');
?>
--EXPECTF--
fread(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+1: fread(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+2: fread(): Argument #2 ($length) must be less than or equal to %d
STR_MAX+3: fread(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX-2: fread(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX-1: fread(): Argument #2 ($length) must be less than or equal to %d
SIZE_MAX: fread(): Argument #2 ($length) must be less than or equal to %d
