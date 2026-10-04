--TEST--
iptcembed() keeps APP13 headers local during reentrant output handling
--FILE--
<?php
$file = __DIR__ . '/iptcembed_reentrant.jpg';
file_put_contents($file, "\xff\xd8\xff\xe0\x00\x02\xff\xda\x00\x02");
$output = '';
$nested = false;
ob_start(function ($chunk) use (&$output, &$nested, $file) {
    $output .= $chunk;
    if (!$nested && str_ends_with($output, "\xff\xed")) {
        $nested = true;
        iptcembed(str_repeat('B', 256), $file, 0);
    }
    return '';
}, 1);
iptcembed('AA', $file, 2);
ob_end_flush();
$start = strpos($output, "\xff\xed");
echo "Nested call: ";
var_dump($nested);
echo "APP13 length: ";
var_dump(unpack('n', substr($output, $start + 2, 2))[1]);
echo "Outer payload: ";
var_dump(substr($output, $start + 30, 2));
?>
--CLEAN--
<?php
unlink(__DIR__ . '/iptcembed_reentrant.jpg');
?>
--EXPECT--
Nested call: bool(true)
APP13 length: int(30)
Outer payload: string(2) "AA"
