--TEST--
JIT DEFINED: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.jit=1235
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($i) {
    $a = defined("X");
    $b = defined("X");
    if (defined("X")) {
        $c = 1;
    } else {
        $c = 0;
    }
    if (!defined("X")) {
        $d = 0;
    } else {
        $d = 1;
    }
    if ($a || $b || $c || $d) {

        die("Error on $i-th iteration\n");
    }

}
for ($i = 0; $i < 10000; $i++) {
    foo($i);
}
echo "ok\n";
?>
--EXPECT--
ok
