--TEST--
Bug #80959: infinite loop in building cfg during JIT compilation
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=tracing
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test($a, $b) {
    echo "Start\n";
    $i = $j = 0;
    do {
        $i++;
        try {
           continue;
        } catch (Exception $e) {
        }
        do {
           $j++;
        } while ($j < $b);
    } while ($i < $a);
    echo "Done $i $j\n";
}
test(5, 6);
?>
--EXPECT--
Start
Done 5 0
