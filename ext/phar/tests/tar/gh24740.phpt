--TEST--
GH-24740 (Unbounded size of .phar/.metadata* entry in TAR phar)
--EXTENSIONS--
phar
--INI--
memory_limit=16M
--FILE--
<?php
function tar_header(string $name, int $size): string
{
    $header = str_repeat("\0", 512);
    $header = substr_replace($header, $name, 0, strlen($name));
    $header = substr_replace($header, sprintf("%07o\0", 0644), 100, 8);
    $header = substr_replace($header, sprintf("%07o\0", 0), 108, 8);
    $header = substr_replace($header, sprintf("%07o\0", 0), 116, 8);
    $header = substr_replace($header, sprintf("%011o\0", $size), 124, 12);
    $header = substr_replace($header, sprintf("%011o\0", 1700000000), 136, 12);
    $header[156] = '0';
    $header = substr_replace($header, "ustar\0", 257, 6);
    $header = substr_replace($header, "00", 263, 2);

    $checksum = 0;
    for ($i = 0; $i < 512; $i++) {
        $checksum += ($i >= 148 && $i < 156) ? 0x20 : ord($header[$i]);
    }

    return substr_replace($header, sprintf("%06o\0 ", $checksum), 148, 8);
}

$fname = __DIR__ . '/gh24740.tar';
file_put_contents($fname, tar_header('.phar/.metadata.bin', 0x7FFFFE00) . str_repeat("\0", 1024));

try {
    new PharData($fname);
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24740.tar');
?>
--EXPECTF--
phar error: tar-based phar "%sgh24740.tar" has invalid metadata in magic file ".phar/.metadata.bin"
