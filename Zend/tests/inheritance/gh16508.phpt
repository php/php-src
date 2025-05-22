--TEST--
GH-16508: Missing lineno in inheritance errors of delayed early bound classes
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

new Test2;

class Test2 extends Test {}

abstract class Test {
    abstract function foo();
}

?>
--EXPECTF--
Fatal error: Class Test2 contains 1 abstract method and must therefore be declared abstract or implement the remaining method (Test::foo) in %s on line 5
