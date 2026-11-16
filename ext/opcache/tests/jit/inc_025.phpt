--TEST--
JIT INC: 025 (result type of ++/-- on a reference after the trace saw a float)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
function pre_inc($a)  { $v = $a; $x = &$v; $out = []; for ($i = 0; $i < 4; $i++) { $out[] = ++$x; } return $out; }
function post_inc($a) { $v = $a; $x = &$v; $out = []; for ($i = 0; $i < 4; $i++) { $out[] = $x++; } return $out; }
function pre_dec($a)  { $v = $a; $x = &$v; $out = []; for ($i = 0; $i < 4; $i++) { $out[] = --$x; } return $out; }
function glob_inc($a) { global $g; $g = $a; $out = []; for ($i = 0; $i < 4; $i++) { $out[] = ++$g; } return $out; }
function stat_inc($a) { static $s; $s = $a; $out = []; for ($i = 0; $i < 4; $i++) { $out[] = ++$s; } return $out; }

class P { public float|string $t = ''; }
function typed_ref_inc($a) {
    $o = new P; $o->t = $a; $x = &$o->t; $out = [];
    for ($i = 0; $i < 4; $i++) { $out[] = $x++; }
    return $out;
}

foreach (['pre_inc', 'post_inc', 'pre_dec', 'glob_inc', 'stat_inc'] as $f) {
    for ($n = 0; $n < 20; $n++) {
        $f(1.5);
    }
    echo $f, ' ', json_encode($f(5)), "\n";
}
for ($n = 0; $n < 20; $n++) {
    typed_ref_inc(1.5);
}
echo 'typed_ref_inc ', json_encode(typed_ref_inc("2.5")), "\n";
?>
--EXPECT--
pre_inc [6,7,8,9]
post_inc [5,6,7,8]
pre_dec [4,3,2,1]
glob_inc [6,7,8,9]
stat_inc [6,7,8,9]
typed_ref_inc ["2.5",3.5,4.5,5.5]
