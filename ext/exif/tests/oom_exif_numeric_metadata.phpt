--TEST--
exif_read_data() with expanded numeric metadata doesn't reach hard limit
--EXTENSIONS--
exif
--INI--
memory_limit=128M
--FILE--
<?php
$base = file_get_contents(__DIR__ . '/image011.jpg');
$entryCount = 900;
$componentCount = 12000;
$dataOffset = 8 + 2 + $entryCount * 12 + 4;

$entries = '';
for ($i = 0; $i < $entryCount; $i++) {
    // Each unique tag retains its own numeric list, while all tags share the
    // same in-bounds payload in the JPEG's TIFF data.
    $entries .= pack('vvVV', 0xf000 + $i, 3, $componentCount, $dataOffset);
}

$tiff = "II*\0" . pack('V', 8)
    . pack('v', $entryCount) . $entries . pack('V', 0)
    . str_repeat("\0", $componentCount * 2);
$app1 = "Exif\0\0" . $tiff;
$jpeg = "\xff\xd8\xff\xe1" . pack('n', strlen($app1) + 2) . $app1 . substr($base, 2);

$file = __DIR__ . '/oom_exif_numeric_metadata.jpg';
file_put_contents($file, $jpeg);
unset($base, $entries, $tiff, $app1, $jpeg);

exif_read_data($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/oom_exif_numeric_metadata.jpg');
?>
--EXPECTF--
Fatal error: Allowed memory size of 134217728 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
