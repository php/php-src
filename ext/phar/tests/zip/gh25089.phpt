--TEST--
GH-25089 (Heap buffer over-read via crafted alias size in zip-based phar)
--EXTENSIONS--
phar
zlib
--FILE--
<?php
$name = '.phar/alias.txt';
$alias = 'ABCD';
$data = gzdeflate($alias);
$crc = crc32($alias);
$claimed = 0x1000;

$local = "PK\x03\x04" . pack('vvvvvVVVvv', 20, 0, 8, 0, 0, $crc, strlen($data), $claimed, strlen($name), 0)
    . $name . $data;
$central = "PK\x01\x02" . pack('vvvvvvVVVvvvvvVV', 20, 20, 0, 8, 0, 0, $crc, strlen($data), $claimed, strlen($name), 0, 0, 0, 0, 0, 0)
    . $name;
$end = "PK\x05\x06" . pack('vvvvVVv', 0, 0, 1, 1, strlen($central), strlen($local), 0);

file_put_contents(__DIR__ . '/gh25089.zip', $local . $central . $end);

$phar = new PharData(__DIR__ . '/gh25089.zip');
var_dump($phar->getAlias());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh25089.zip');
?>
--EXPECT--
string(4) "ABCD"
