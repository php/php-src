--TEST--
imagecreatefromxbm() rejects out-of-range XBM dimensions
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (!GD_BUNDLED) {
    die("skip dimension range checks require bundled GD");
}
?>
--FILE--
<?php
$path = __DIR__ . '/xbm_out_of_range.tmp.xbm';
foreach ([['4294967298', '1'], ['1', '4294967298']] as [$w, $h]) {
    file_put_contents($path, "#define x_width $w\n#define x_height $h\nstatic char x_bits[] = {\n0x00, 0x00 };\n");
    echo "$w x $h\n";
    var_dump(imagecreatefromxbm($path));
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/xbm_out_of_range.tmp.xbm');
?>
--EXPECTF--
4294967298 x 1

Warning: imagecreatefromxbm(): "%s" is not a valid XBM file in %s on line %d
bool(false)
1 x 4294967298

Warning: imagecreatefromxbm(): "%s" is not a valid XBM file in %s on line %d
bool(false)
