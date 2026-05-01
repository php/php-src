--TEST--
Op array fixed up multiple times during preloading
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.preload={PWD}/preload_trait_multiple_fixup.inc
--EXTENSIONS--
opcache
--FILE--
<?php
(new C2)->method();
(new C4)->method();
?>
--EXPECT--
Foo
Foo
