--TEST--
GH-24242: Reject truncated tar entries whose size overflows a 32-bit stream offset
--EXTENSIONS--
phar
--FILE--
<?php
$filename = __DIR__ . '/gh24242.tar';

$header = str_pad('bigfile', 100, "\0")
    . "0000644\0"
    . "0000000\0"
    . "0000000\0"
    . "30000000000\0"
    . "00000000000\0"
    . str_repeat(' ', 8)
    . '0';
$header = str_pad($header, 512, "\0");
$checksum = array_sum(unpack('C*', $header));
$header = substr_replace($header, sprintf("%06o\0 ", $checksum), 148, 8);

file_put_contents($filename, $header . str_repeat("\0", 16384));

try {
    $phar = new PharData($filename);
    echo 'Parsed, entries: ', count($phar), "\n";
    foreach ($phar as $file) {
        echo 'Name: ', $file->getFilename(), "\n";
        echo 'Size: ', $file->getSize(), "\n";
    }
} catch (UnexpectedValueException $e) {
    echo $e->getMessage(), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh24242.tar');
?>
--EXPECTF--
phar error: "%sgh24242.tar" is a corrupted tar file (%s)
