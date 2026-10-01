--TEST--
GHSA-j3wh-g957-2m85 (././@LongLink name longer than the archive)
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
function tar_header(string $name, string $size, string $typeflag = '0'): string
{
    $header = str_repeat("\0", 512);
    $header = substr_replace($header, $name, 0, strlen($name));
    $header = substr_replace($header, sprintf("%07o\0", 0644), 100, 8);
    $header = substr_replace($header, sprintf("%07o\0", 0), 108, 8);
    $header = substr_replace($header, sprintf("%07o\0", 0), 116, 8);
    $header = substr_replace($header, $size, 124, 12);
    $header = substr_replace($header, sprintf("%011o\0", 1700000000), 136, 12);
    $header[156] = $typeflag;
    $header = substr_replace($header, "ustar\0", 257, 6);
    $header = substr_replace($header, "00", 263, 2);

    $checksum = 0;
    for ($i = 0; $i < 512; $i++) {
        $checksum += ($i >= 148 && $i < 156) ? 0x20 : ord($header[$i]);
    }

    return substr_replace($header, sprintf("%06o\0 ", $checksum), 148, 8);
}

$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';

/* 2 GB of file name announced by a 2 KB archive */
$tar  = tar_header('././@LongLink', "20000000000\0", 'L');
$tar .= str_pad('long.txt', 512, "\0");
$tar .= tar_header('short.txt', sprintf("%011o\0", 3)) . str_pad('abc', 512, "\0");
$tar .= str_repeat("\0", 1024);
file_put_contents($fname, $tar);

try {
    new PharData($fname);
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
phar error: "%s" is a corrupted tar file (invalid entry size)
