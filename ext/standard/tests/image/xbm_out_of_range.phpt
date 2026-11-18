--TEST--
getimagesize() rejects out-of-range XBM dimensions
--FILE--
<?php
$path = __DIR__ . '/xbm_out_of_range.tmp.xbm';
foreach ([['4294967298', '1'], ['1', '4294967298'], ['2147483648', '1'], ['-5', '1'], ['2147483647', '1']] as [$w, $h]) {
    file_put_contents($path, "#define x_width $w\n#define x_height $h\nstatic char x_bits[] = {\n0x00 };\n");
    $info = getimagesize($path);
    echo "$w x $h: ", $info ? $info[3] : 'false', "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/xbm_out_of_range.tmp.xbm');
?>
--EXPECT--
4294967298 x 1: false
1 x 4294967298: false
2147483648 x 1: false
-5 x 1: false
2147483647 x 1: width="2147483647" height="1"
