--TEST--
Type inference 003: instanceof with unlinked class
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
class C implements I {
    public static function wrap(\Throwable $e): C {
        return $e instanceof C ? $e : new C($e->getMessage());
    }
}
?>
--EXPECTF--
Fatal error: Uncaught Error: Interface "I" not found in %sinference_003.php:2
Stack trace:
#0 {main}
  thrown in %sinference_003.php on line 2

