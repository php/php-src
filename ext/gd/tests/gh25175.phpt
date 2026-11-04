--TEST--
GH-25175 (imagecopyresampled() copies nothing with negative source width/height)
--EXTENSIONS--
gd
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
?>
--EXPECT--
0000ff ff0000 0000ff 0000ff
0000ff 0000ff ff0000 0000ff
0000ff 0000ff 0000ff ff0000
