--TEST--
Bug #77857 (Wrong result if executed with JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64
opcache.jit=1205
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test() {
    $arr = array_fill(0, 1, 1.0);
    $y = 0.0;
    foreach ($arr as $v) {
        $tmp = 1.0 * $v;
        var_dump($tmp);
        $y = $tmp/1.0;
    }
    return $y;
}
var_dump(test());
?>
--EXPECT--
float(1)
float(1)
