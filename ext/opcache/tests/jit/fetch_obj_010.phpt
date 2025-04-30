--TEST--
JIT: FETCH_OBJ 010
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo() {
    for($cnt=0;$cnt<3;$cnt++) {
        $obj->ary["bas"] ??= $obj = new stdClass;
    }
}
foo();
?>
DONE
--EXPECT--
DONE
