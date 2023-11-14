--TEST--
JIT ASSIGN: 025
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $arr[0][0] = $ref;
    for($cnt=0;$cnt<6;$cnt++) {
        $ref = 1;
        $arr[0][0] = $ref;
    }
}
foo();
echo "ok\n";
?>
--EXPECTF--
Warning: Undefined variable $ref in %s on line %d
ok
