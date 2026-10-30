--TEST--
GH-20863 (getimagesize() cannot read dimensions of many HEIC images)
--FILE--
<?php
function box(string $type, string $content): string {
    return pack('N', 8 + strlen($content)) . $type . $content;
}
function fullbox(string $type, int $version, int $flags, string $content): string {
    return box($type, pack('N', ($version << 24) | $flags) . $content);
}
function ftyp(): string {
    return box('ftyp', 'heic' . pack('N', 0) . 'heic' . 'mif1');
}

// HEIC grid image whose primary item's property association sits past the
// number of tile items, where libavifinfo used to stop recording them.
function make_grid(int $w, int $h, int $tiles = 30): string {
    $ipco = box('ipco',
        fullbox('ispe', 0, 0, pack('NN', 64, 64)) .
        fullbox('ispe', 0, 0, pack('NN', $w, $h)) .
        fullbox('pixi', 0, 0, pack('C4', 3, 8, 8, 8)));
    $primary = $tiles + 1;
    $entries = '';
    for ($id = 1; $id <= $tiles; $id++) {
        $entries .= pack('n', $id) . pack('C', 2) . pack('C2', 1, 3);
    }
    $entries .= pack('n', $primary) . pack('C', 2) . pack('C2', 2, 3);
    $ipma = fullbox('ipma', 0, 0, pack('N', $primary) . $entries);
    $iprp = box('iprp', $ipco . $ipma);
    $infes = '';
    for ($id = 1; $id <= $primary; $id++) {
        $infes .= fullbox('infe', 2, 0, pack('n', $id) . pack('n', 0) . 'hvc1');
    }
    $iinf = fullbox('iinf', 0, 0, pack('n', $primary) . $infes);
    $pitm = fullbox('pitm', 0, 0, pack('n', $primary));
    return ftyp() . fullbox('meta', 0, 0, $pitm . $iinf . $iprp);
}

// HEIC without image properties libavifinfo can use, but with an Exif item
// that carries the dimensions.
function make_exif(int $w, int $h): string {
    $tiff = 'II' . pack('v', 42) . pack('V', 8) .
        pack('v', 2) .
        pack('vvV', 0x0100, 4, 1) . pack('V', $w) .
        pack('vvV', 0x0101, 4, 1) . pack('V', $h) .
        pack('V', 0);
    $payload = pack('N', 0) . $tiff;
    $pitm = fullbox('pitm', 0, 0, pack('n', 1));
    $iinf = fullbox('iinf', 0, 0, pack('n', 2) .
        fullbox('infe', 2, 0, pack('n', 1) . pack('n', 0) . 'hvc1') .
        fullbox('infe', 2, 0, pack('n', 2) . pack('n', 0) . 'Exif'));
    $iloc = function (int $off) use ($payload) {
        return fullbox('iloc', 1, 0,
            pack('C2', 0x44, 0x00) . pack('n', 1) .
            pack('n', 2) . pack('n', 0) . pack('n', 0) . pack('n', 1) .
            pack('N', $off) . pack('N', strlen($payload)));
    };
    $off = strlen(ftyp()) + strlen(fullbox('meta', 0, 0, $pitm . $iinf . $iloc(0))) + 8;
    return ftyp() . fullbox('meta', 0, 0, $pitm . $iinf . $iloc($off)) . box('mdat', $payload);
}

$grid = tempnam(sys_get_temp_dir(), 'heic');
file_put_contents($grid, make_grid(1920, 1080));
$exif = tempnam(sys_get_temp_dir(), 'heic');
file_put_contents($exif, make_exif(1234, 567));

$g = getimagesize($grid);
echo "grid: {$g[0]}x{$g[1]} {$g['mime']} bits={$g['bits']} channels={$g['channels']}\n";
$e = getimagesize($exif);
echo "exif: {$e[0]}x{$e[1]} {$e['mime']}\n";

unlink($grid);
unlink($exif);
?>
--EXPECT--
grid: 1920x1080 image/heif bits=8 channels=3
exif: 1234x567 image/heif
