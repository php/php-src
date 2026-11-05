--TEST--
GH-24471 (Tracing JIT: CSE'd float temporary marked type_only in trace exit)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.file_update_protection=0
--FILE--
<?php
function f($x, $y) {
    $r = ($x + 0.5) + ($d = $x + 0.5) * ($y === [1] ? 2.0 : 3.0);
    return $r + $d;
}
$ys = array_fill(0, 100, [0]);
$ys[] = [1];
foreach ($ys as $i => $y) {
    $r = f($i, $y);
}
var_dump($r);
?>
--EXPECT--
float(402)
