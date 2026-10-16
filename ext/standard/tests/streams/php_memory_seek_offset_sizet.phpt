--TEST--
fseek() on php://memory with offsets beyond size_t on narrow size_t
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip size_t is not narrower than zend_long on this platform");
}
?>
--FILE--
<?php
$sizeMax = 2 ** (PHP_SYS_SIZE * 8) - 1;

foreach (['php://memory', 'php://temp'] as $url) {
    echo "$url\n";
    $s = fopen($url, 'r+');
    fwrite($s, 'hello');

    foreach ([
        'SET SIZE_MAX + 1' => [$sizeMax + 1, SEEK_SET],
        'SET SIZE_MAX + 3' => [$sizeMax + 3, SEEK_SET],
        'SET PHP_INT_MAX' => [PHP_INT_MAX, SEEK_SET],
        'CUR SIZE_MAX' => [$sizeMax, SEEK_CUR],
        'CUR PHP_INT_MAX' => [PHP_INT_MAX, SEEK_CUR],
        'CUR -(SIZE_MAX + 1)' => [-($sizeMax + 1), SEEK_CUR],
        'CUR PHP_INT_MIN' => [PHP_INT_MIN, SEEK_CUR],
        'END SIZE_MAX' => [$sizeMax, SEEK_END],
        'END -(SIZE_MAX + 1)' => [-($sizeMax + 1), SEEK_END],
        'END PHP_INT_MIN' => [PHP_INT_MIN, SEEK_END],
    ] as $label => [$offset, $whence]) {
        fseek($s, 1);
        printf("%-20s %2d %d\n", $label, fseek($s, $offset, $whence), ftell($s));
    }

    echo "Up to SIZE_MAX\n";
    var_dump(fseek($s, $sizeMax, SEEK_SET));
    var_dump(ftell($s) === $sizeMax);
    var_dump(fseek($s, 1, SEEK_CUR));
    var_dump(ftell($s) === $sizeMax);
    var_dump(fseek($s, $sizeMax - 5, SEEK_END));
    var_dump(ftell($s) === $sizeMax);

    fclose($s);
}
?>
--EXPECT--
php://memory
SET SIZE_MAX + 1     -1 1
SET SIZE_MAX + 3     -1 1
SET PHP_INT_MAX      -1 1
CUR SIZE_MAX         -1 1
CUR PHP_INT_MAX      -1 1
CUR -(SIZE_MAX + 1)  -1 1
CUR PHP_INT_MIN      -1 1
END SIZE_MAX         -1 1
END -(SIZE_MAX + 1)  -1 1
END PHP_INT_MIN      -1 1
Up to SIZE_MAX
int(0)
bool(true)
int(-1)
bool(true)
int(0)
bool(true)
php://temp
SET SIZE_MAX + 1     -1 1
SET SIZE_MAX + 3     -1 1
SET PHP_INT_MAX      -1 1
CUR SIZE_MAX         -1 1
CUR PHP_INT_MAX      -1 1
CUR -(SIZE_MAX + 1)  -1 1
CUR PHP_INT_MIN      -1 1
END SIZE_MAX         -1 1
END -(SIZE_MAX + 1)  -1 1
END PHP_INT_MIN      -1 1
Up to SIZE_MAX
int(0)
bool(true)
int(-1)
bool(true)
int(0)
bool(true)
