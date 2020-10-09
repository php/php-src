--TEST--
JIT NOVAL: 001 (bug in zend_jit_compute_false_dependencies())
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class Foo {
    const X = false;

    static function bar() {
        $count = 0;
        if (self::X) {
            $count = intval(9223372036854775807);
        }
        if (self::X) {
           $count = 2;
        }
        if ($count != 0) {
            return "bug";
        }
        return "ok";
    }
}
var_dump(Foo::bar());
?>
--EXPECT--
string(2) "ok"

