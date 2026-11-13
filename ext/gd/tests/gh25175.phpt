--TEST--
GH-25175 (imagecopyresampled() copies nothing with negative source width/height)
--EXTENSIONS--
gd
--CREDITS--
desrosj
--FILE--
<?php
$src = imagecreatetruecolor(100, 100);
imagefilledrectangle($src, 0, 0, 99, 99, 0x0000ff);
imagefilledrectangle($src, 0, 0, 9, 9, 0xff0000);

$cases = [
    [99, 0, -100, 100],
    [0, 99, 100, -100],
    [99, 99, -100, -100],
];

foreach ($cases as [$src_x, $src_y, $src_w, $src_h]) {
    $dst = imagecreatetruecolor(100, 100);
    imagecopyresampled($dst, $src, 0, 0, $src_x, $src_y, 100, 100, $src_w, $src_h);
    printf(
        "%06x %06x %06x %06x\n",
        imagecolorat($dst, 0, 0),
        imagecolorat($dst, 99, 0),
        imagecolorat($dst, 0, 99),
        imagecolorat($dst, 99, 99)
    );
}

$src = imagecreatetruecolor(4, 1);
foreach ([0x100000, 0x200000, 0x300000, 0x400000] as $x => $color) {
    imagesetpixel($src, $x, 0, $color);
}
$dst = imagecreatetruecolor(8, 1);
imagecopyresampled($dst, $src, 0, 0, 3, 0, 8, 1, -4, 1);
for ($x = 0; $x < 8; $x++) {
    printf("%06x ", imagecolorat($dst, $x, 0));
}
echo "\n";
?>
--EXPECT--
0000ff ff0000 0000ff 0000ff
0000ff 0000ff ff0000 0000ff
0000ff 0000ff 0000ff ff0000
400000 400000 300000 300000 200000 200000 100000 100000
