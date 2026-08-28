--TEST--
fseek(): an invalid $whence that fits in an int must not desynchronize the stream
--FILE--
<?php
$tmp = __DIR__ . '/fseek_whence_invalid_inrange.tmp';
file_put_contents($tmp, "0123456789");

foreach ([99, -2147483648, 2147483647] as $whence) {
    echo 'whence=', $whence, PHP_EOL;
    $h = fopen($tmp, 'r');
    var_dump(fread($h, 4));
    var_dump(fseek($h, 3, $whence));
    var_dump(ftell($h));
    var_dump(fread($h, 6));
    fclose($h);
    echo PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/fseek_whence_invalid_inrange.tmp');
?>
--EXPECT--
whence=99
string(4) "0123"
int(-1)
int(4)
string(6) "456789"

whence=-2147483648
string(4) "0123"
int(-1)
int(4)
string(6) "456789"

whence=2147483647
string(4) "0123"
int(-1)
int(4)
string(6) "456789"
