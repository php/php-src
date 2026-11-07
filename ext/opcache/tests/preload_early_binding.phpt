--TEST--
Early binding should work fine inside the preload script
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_early_binding.inc
--EXTENSIONS--
opcache
--FILE--
OK
--EXPECT--
object(X)#1 (0) {
}
OK
