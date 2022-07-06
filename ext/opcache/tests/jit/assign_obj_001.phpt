--TEST--
JIT ASSIGN_OBJ: Assign property on null
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function test1($o) {
    $o->x = new stdClass;
}
function test2($o) {
    $o->x += new stdClass;
}

try {
    test1(null);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    test2(null);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Attempt to assign property "x" on null
Attempt to assign property "x" on null
