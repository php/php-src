--TEST--
GH-23477 (Memory leak on duplicate native Phar manifest entry)
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
$stub = "<?php __HALT_COMPILER(); ?>\r\n";

function u32($value) {
    return pack('V', $value);
}

function entry($name, $data, $metadata) {
    $header = u32(strlen($name)) . $name
        . u32(strlen($data)) . u32(0) . u32(strlen($data))
        . u32(crc32($data)) . u32(0)
        . u32(strlen($metadata)) . $metadata;
    return [$header, $data];
}

$first = entry('a.txt', 'hello', 'i:1;');
$second = entry('a.txt', 'world', 'i:2;');
$manifest = u32(2) . "\x11\x00" . u32(0) . u32(0) . u32(0)
    . $first[0] . $second[0];

file_put_contents(__DIR__ . '/gh23477.phar',
    $stub . u32(strlen($manifest)) . $manifest . $first[1] . $second[1]);

$phar = new Phar(__DIR__ . '/gh23477.phar');
echo iterator_count($phar), "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh23477.phar');
?>
--EXPECT--
1
