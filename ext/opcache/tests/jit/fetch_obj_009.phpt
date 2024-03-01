--TEST--
JIT: FETCH_OBJ 009
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function test() {
    for ($i = 0; $i < 10; $i++) {
        $obj = new stdClass;
        $obj->x[0] = null;
        $obj->x > $obj->x[0] = null;
    }
}
test();
?>
DONE
--EXPECT--
DONE
