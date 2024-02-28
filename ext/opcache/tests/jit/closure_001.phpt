--TEST--
Closures should be always called with ZEND_ACC_CLOSURE flag set 
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
opcache.jit_hot_func=2
--EXTENSIONS--
opcache
--FILE--
<?php
class Foo {
}

function bar() {
    return function () {
        return function () {
            return function () {
                return 42;
            };
        };
    };
}

$foo = new Foo;
$f = bar();

var_dump($f->call($foo));
var_dump($f->call($foo));
var_dump($f());
?>
--EXPECTF--
object(Closure)#3 (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["this"]=>
  object(Foo)#1 (0) {
  }
}
object(Closure)#3 (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["this"]=>
  object(Foo)#1 (0) {
  }
}
object(Closure)#3 (1) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
}
