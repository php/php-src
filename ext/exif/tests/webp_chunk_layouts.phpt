--TEST--
exif_read_data() on WebP files: no EXIF chunk, chunk padding and malformed layouts
--EXTENSIONS--
exif
--INI--
output_handler=
zlib.output_compression=0
--FILE--
<?php
function chunk(string $id, string $data): string {
    return $id . pack('V', strlen($data)) . $data . (strlen($data) & 1 ? "\0" : '');
}
function webp(array $chunks, ?int $riffSize = null): string {
    $body = 'WEBP' . implode('', $chunks);
    return 'RIFF' . pack('V', $riffSize ?? strlen($body)) . $body;
}
// Little-endian TIFF block with a single IFD0 entry: Orientation = 6.
$tiff = "II*\0" . pack('V', 8) . pack('v', 1) . pack('vvVv', 0x0112, 3, 1, 6) . "\0\0" . pack('V', 0);
$image = chunk('VP8L', "/\0\0\0\0\0\0\0\0\0");
$vp8x = chunk('VP8X', "\x08" . str_repeat("\0", 9));

$cases = [
    'no EXIF chunk' => webp([$image]),
    'no EXIF chunk, VP8X' => webp([$vp8x, $image]),
    'EXIF after image' => webp([$vp8x, $image, chunk('EXIF', $tiff)]),
    'EXIF with Exif header' => webp([$vp8x, $image, chunk('EXIF', "Exif\0\0" . $tiff)]),
    'EXIF after odd sized chunk' => webp([$vp8x, chunk('XMP ', 'abc'), $image, chunk('EXIF', $tiff)]),
    'RIFF size larger than file' => webp([$vp8x, $image, chunk('EXIF', $tiff)], 0x7fffffff),
    'truncated chunk' => substr(webp([$vp8x, $image, chunk('EXIF', $tiff)]), 0, -10),
    'RIFF size too small for EXIF' => webp([$vp8x, $image, chunk('EXIF', $tiff)], 40),
    'EXIF chunk too small' => webp([$vp8x, $image, chunk('EXIF', 'Exif')]),
];

$file = __DIR__ . '/exif_webp_layouts.webp';
foreach ($cases as $name => $bytes) {
    file_put_contents($file, $bytes);
    echo $name, ":\n";
    $result = exif_read_data($file, null, true);
    if ($result === false) {
        echo "  false\n";
        continue;
    }
    echo '  ', $result['FILE']['MimeType'], ' ', implode(',', array_keys($result)),
        ' Orientation=', $result['IFD0']['Orientation'] ?? 'none', "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/exif_webp_layouts.webp');
?>
--EXPECTF--
no EXIF chunk:
  image/webp FILE,COMPUTED Orientation=none
no EXIF chunk, VP8X:
  image/webp FILE,COMPUTED Orientation=none
EXIF after image:
  image/webp FILE,COMPUTED,IFD0 Orientation=6
EXIF with Exif header:
  image/webp FILE,COMPUTED,IFD0 Orientation=6
EXIF after odd sized chunk:
  image/webp FILE,COMPUTED,IFD0 Orientation=6
RIFF size larger than file:
  image/webp FILE,COMPUTED,IFD0 Orientation=6
truncated chunk:

Warning: exif_read_data(): Invalid WebP file in %s on line %d
  false
RIFF size too small for EXIF:
  image/webp FILE,COMPUTED Orientation=none
EXIF chunk too small:

Warning: exif_read_data(): Invalid WebP file in %s on line %d
  false