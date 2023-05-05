--TEST--
Observer: Observe function and class declarations with file_cache_only
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_declaring=1
opcache.enable_cli=1
opcache.file_cache="{TMP}"
opcache.file_cache_only=1
--FILE--
<?php

function a() {}
class A {}
class B extends A {}

?>
--EXPECTF--
<!-- declared function 'a' -->
<!-- declared class 'a' -->
<!-- declared class 'b' -->
<!-- init '%s' -->
