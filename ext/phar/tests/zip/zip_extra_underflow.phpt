--TEST--
Phar: ZIP extra field length must not underflow
--EXTENSIONS--
phar
--FILE--
<?php
function uint16($value) {
    return pack('v', $value);
}

function uint32($value) {
    return pack('V', $value);
}

$filename = __DIR__ . '/zip_extra_underflow.zip';
$entry = 'test.txt';
$contents = 'hello';
$crc = crc32($contents);

$local = uint32(0x04034b50)
    . uint16(20)
    . uint16(0)
    . uint16(0)
    . uint16(0)
    . uint16(0)
    . uint32($crc)
    . uint32(strlen($contents))
    . uint32(strlen($contents))
    . uint16(strlen($entry))
    . uint16(0)
    . $entry
    . $contents;

$extra = 'XX' . uint16(1);

/* Old code seeks one byte past the extra field and parses this as another extra header. */
$commentPrefix = 'A'
    . 'UT'
    . uint16(5)
    . "\x01"
    . uint32(946684800)
    . 'ZZ'
    . uint16(65522);
$comment = $commentPrefix . str_repeat('B', 65535 - strlen($commentPrefix));

$central = uint32(0x02014b50)
    . uint16(20)
    . uint16(20)
    . uint16(0)
    . uint16(0)
    . uint16(0)
    . uint16(0)
    . uint32($crc)
    . uint32(strlen($contents))
    . uint32(strlen($contents))
    . uint16(strlen($entry))
    . uint16(strlen($extra))
    . uint16(strlen($comment))
    . uint16(0)
    . uint16(0)
    . uint32(0)
    . uint32(0)
    . $entry
    . $extra
    . $comment;

$eocd = uint32(0x06054b50)
    . uint16(0)
    . uint16(0)
    . uint16(1)
    . uint16(1)
    . uint32(strlen($central))
    . uint32(strlen($local))
    . uint16(0);

file_put_contents($filename, $local . $central . $eocd);

try {
    $phar = new PharData($filename);
    echo "Loaded corrupt ZIP\n";
    echo $phar[$entry]->getMTime(), "\n";
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/zip_extra_underflow.zip');
?>
--EXPECTF--
UnexpectedValueException: phar error: Unable to process extra field header for file in central directory in zip-based phar "%szip_extra_underflow.zip"
