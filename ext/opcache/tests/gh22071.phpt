--TEST--
GH-22071: Assertion failure jit_CONST_FUNC_PROTO on abstract static method call
--CREDITS--
YuanchengJiang
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1205
opcache.jit_buffer_size=16M
--FILE--
<?php
try {
    $e = enum_exists('UnitEnum') ? UnitEnum::cases() : [];
} catch (\Throwable $_e) {}
echo "ok\n";
?>
--EXPECT--
ok
