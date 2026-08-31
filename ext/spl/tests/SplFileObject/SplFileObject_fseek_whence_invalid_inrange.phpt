--TEST--
SplFileObject::fseek(): a failed seek must not discard the current line
--FILE--
<?php
$tmp = __DIR__ . '/SplFileObject_fseek_whence_invalid_inrange.tmp';
file_put_contents($tmp, "aaa\nbbb\nccc\n");

foreach ([99, -2147483648, 2147483647] as $whence) {
    echo 'whence=', $whence, PHP_EOL;

    $file = new SplFileObject($tmp);
    var_dump($file->current());
    var_dump($file->fseek(3, $whence));
    var_dump($file->ftell());
    var_dump($file->current());
    unset($file);

    echo PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/SplFileObject_fseek_whence_invalid_inrange.tmp');
?>
--EXPECT--
whence=99
string(4) "aaa
"
int(-1)
int(4)
string(4) "aaa
"

whence=-2147483648
string(4) "aaa
"
int(-1)
int(4)
string(4) "aaa
"

whence=2147483647
string(4) "aaa
"
int(-1)
int(4)
string(4) "aaa
"
