--TEST--
GH-20584 (Information Leak of Memory)
--CREDITS--
Nikita Sveshnikov (Positive Technologies)
--FILE--
<?php
// Minimal PoC: corruption/uninitialized memory leak when reading APP1 via php://filter
$file = __DIR__ . '/gh20584.jpg';

// Make APP1 large enough so it is read in multiple chunks
$chunk = 8192;
$tail = 123;
$payload = str_repeat('A', $chunk) . str_repeat('B', $chunk) . str_repeat('Z',
$tail);
$app1Len = 2 + strlen($payload);

// Minimal JPEG: SOI + APP1 + SOF0(1x1) + EOI
$sof = "\xFF\xC0" . pack('n', 11) . "\x08" . pack('n',1) . pack('n',1) .
"\x01\x11\x00";
$jpeg = "\xFF\xD8" . "\xFF\xE1" . pack('n', $app1Len) . $payload . $sof .
"\xFF\xD9";
file_put_contents($file, $jpeg);

// Read through a filter to enforce multiple reads
$src = 'php://filter/read=string.rot13|string.rot13/resource=' . $file;
$info = null;
@getimagesize($src, $info);
$exp = $payload;
$ret = $info['APP1'];

var_dump($ret === $exp);

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh20584.jpg');
?>
--EXPECT--
bool(true)
