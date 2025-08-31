--TEST--
Check that reference detection works properly
--FILE--
<?php

$v00 = $v01 = $v32 = $v33 = 0;
test(p32: $v32, p33: $v33, p00: $v00, p01: $v01);
echo "$v00 $v01 $v32 $v33\n";

$v = [0 => 0, 1 => 0, 32 => 0, 33 => 0];
test(p32: $v[32], p33: $v[33], p00: $v[0], p01: $v[1]);
echo "$v[0] $v[1] $v[32] $v[33]\n";

function test(
    &$p00 = null, $p01 = null, &$p02 = null, $p03 = null, &$p04 = null, $p05 = null,
    &$p06 = null, $p07 = null, &$p08 = null, $p09 = null, &$p10 = null, $p11 = null,
    &$p12 = null, $p13 = null, &$p14 = null, $p15 = null, &$p16 = null, $p17 = null,
    &$p18 = null, $p19 = null, &$p20 = null, $p21 = null, &$p22 = null, $p23 = null,
    &$p24 = null, $p25 = null, &$p26 = null, $p27 = null, &$p28 = null, $p29 = null,
    &$p30 = null, $p31 = null, &$p32 = null, $p33 = null, &$p34 = null, $p35 = null
) {
    $p00++;
    $p32++;
}

$v00 = $v01 = $v32 = $v33 = 0;
test(p32: $v32, p33: $v33, p00: $v00, p01: $v01);
echo "$v00 $v01 $v32 $v33\n";

$v = [0 => 0, 1 => 0, 32 => 0, 33 => 0];
test(p32: $v[32], p33: $v[33], p00: $v[0], p01: $v[1]);
echo "$v[0] $v[1] $v[32] $v[33]\n";

?>
--EXPECT--
1 0 1 0
1 0 1 0
1 0 1 0
1 0 1 0
