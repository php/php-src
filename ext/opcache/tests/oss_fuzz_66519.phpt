--TEST--
oss-fuzz #66519: Fix RC inference narrowing for ASSIGN_OBJ
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
--FILE--
<?php
function test() {
    for (;;) {
        [] ?? $oj->y = y;
        $oj = new stdClass;
    }
}
?>
===DONE===
--EXPECT--
===DONE===
