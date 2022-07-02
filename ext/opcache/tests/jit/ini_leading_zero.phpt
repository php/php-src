--TEST--
Leading zero in opcache.jit option
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=0205
--EXTENSIONS--
opcache
--FILE--
===DONE===
--EXPECT--
===DONE===
