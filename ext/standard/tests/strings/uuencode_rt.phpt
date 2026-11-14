--TEST--
convert_uuencode() and convert_uudecode() round trip
--FILE--
<?php

$vectors = [
    'empty string' => ["", ['`']],
    '1 byte' => ["a", ['!80``', '`']],
    '2 bytes' => ["ab", ['"86(`', '`']],
    '3 bytes' => ["abc", ['#86)C', '`']],
    '4 bytes' => ["abcd", ['$86)C9```', '`']],
    '5 bytes' => ["abcde", ['%86)C9&4`', '`']],
    '6 bytes' => ["abcdef", ['&86)C9&5F', '`']],
    '1 NUL' => ["\x00", ['!````', '`']],
    '2 NULs' => ["\x00\x00", ['"````', '`']],
    '3 NULs' => ["\x00\x00\x00", ['#````', '`']],
    '4 NULs' => ["\x00\x00\x00\x00", ['$````````', '`']],
    '1 x 0xFF' => ["\xff", ['!_P``', '`']],
    '2 x 0xFF' => ["\xff\xff", ['"__\\`', '`']],
    '3 x 0xFF' => ["\xff\xff\xff", ['#____', '`']],
    '4 x 0xFF' => ["\xff\xff\xff\xff", ['$_____P``', '`']],
    '5 x 0xFF' => ["\xff\xff\xff\xff\xff", ['%______\\`', '`']],
    'manual example' => ["test\x0atext text\x0d\x0a", ['0=&5S=`IT97AT(\'1E>\'0-"@``', '`']],
    '44 bytes' => [str_repeat('a', 44), ['L86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86$`', '`']],
    '45 bytes (one full line)' => [str_repeat('a', 45), ['M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', '`']],
    '46 bytes' => [str_repeat('a', 46), ['M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', '!80``', '`']],
    '47 bytes' => [str_repeat('a', 47), ['M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', '"86$`', '`']],
    '48 bytes' => [str_repeat('a', 48), ['M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', '#86%A', '`']],
    '90 bytes (two full lines)' => [str_repeat('a', 90), ['M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', 'M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', '`']],
    '93 bytes' => [str_repeat('a', 93), ['M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', 'M86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A86%A', '#86%A', '`']],
];

$failures = [];
foreach ($vectors as $label => [$input, $lines]) {
    $expected = implode("\n", $lines) . "\n";
    if (convert_uuencode($input) !== $expected) {
        $failures[] = "encode: $label";
    }
    if (convert_uudecode($expected) !== $input) {
        $failures[] = "decode: $label";
    }
}
var_dump($failures);

?>
--EXPECT--
array(0) {
}