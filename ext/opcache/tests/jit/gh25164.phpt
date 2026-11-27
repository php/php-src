--TEST--
GH-25164 (JIT: loop variable kept in a register across a user function call)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php

function leaf() {
    static $calls = 0;
    if (++$calls > 20) {
        die("runaway loop\n");
    }
    echo "a";
}

function driver() {
    $i = 0;
    while ($i++ < 5) {
        leaf();
    }
    return $i;
}

function driver2() {
    $i = 0;
    while ((($i = ($i + 3) & 15) | 0) != 9) {
        leaf();
        echo "-";
    }
    return $i;
}

var_dump(driver());
var_dump(driver2());

?>
--EXPECT--
aaaaaint(6)
a-a-int(9)
