--TEST--
Observer: Closures keep their runtime cache when not rebinding
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php

class Foo {
  private static $value = "x";
  function static() {
    (static function () {
      echo Foo::$value, PHP_EOL;
    })();
  }
  function non_static() {
    (function () {
      echo Foo::$value, PHP_EOL;
    })();
  }
  function rebind() {
    ((function () {
      try {
        echo Foo::$value, PHP_EOL;
      } catch (Error $e) {
        echo $e::class, ": ", $e->getMessage(), PHP_EOL;
      }
    })->bindTo(null, null))();
  }
}

$obj = new Foo();
$obj->static();
$obj->static();
$obj->non_static();
$obj->non_static();
$obj->rebind();
$obj->rebind();

$closure = function () {
  echo Foo::$value, PHP_EOL;
};

($closure->bindTo(null, Foo::class))();
($closure->bindTo(null, Foo::class))();

$boundClosure = $closure->bindTo(null, Foo::class);
$boundClosure();
$boundClosure();

?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init Foo::static() -->
  <Foo::static>
    <!-- init Foo::{closure:Foo::static():6}() -->
    <Foo::{closure:Foo::static():6}>
x
    </Foo::{closure:Foo::static():6}>
  </Foo::static>
  <Foo::static>
    <Foo::{closure:Foo::static():6}>
x
    </Foo::{closure:Foo::static():6}>
  </Foo::static>
  <!-- init Foo::non_static() -->
  <Foo::non_static>
    <!-- init Foo::{closure:Foo::non_static():11}() -->
    <Foo::{closure:Foo::non_static():11}>
x
    </Foo::{closure:Foo::non_static():11}>
  </Foo::non_static>
  <Foo::non_static>
    <Foo::{closure:Foo::non_static():11}>
x
    </Foo::{closure:Foo::non_static():11}>
  </Foo::non_static>
  <!-- init Foo::rebind() -->
  <Foo::rebind>
    <!-- init Closure::bindTo() -->
    <Closure::bindTo>
    </Closure::bindTo>
    <!-- init {closure:Foo::rebind():16}() -->
    <{closure:Foo::rebind():16}>
Error:       <!-- init Error::getMessage() -->
      <Error::getMessage>
      </Error::getMessage>
Cannot access private property Foo::$value
    </{closure:Foo::rebind():16}>
  </Foo::rebind>
  <Foo::rebind>
    <Closure::bindTo>
    </Closure::bindTo>
    <!-- init {closure:Foo::rebind():16}() -->
    <{closure:Foo::rebind():16}>
Error:       <Error::getMessage>
      </Error::getMessage>
Cannot access private property Foo::$value
    </{closure:Foo::rebind():16}>
  </Foo::rebind>
  <Closure::bindTo>
  </Closure::bindTo>
  <!-- init Foo::{closure:%s:%d}() -->
  <Foo::{closure:%s:%d}>
x
  </Foo::{closure:%s:%d}>
  <Closure::bindTo>
  </Closure::bindTo>
  <!-- init Foo::{closure:%s:%d}() -->
  <Foo::{closure:%s:%d}>
x
  </Foo::{closure:%s:%d}>
  <Closure::bindTo>
  </Closure::bindTo>
  <!-- init Foo::{closure:%s:%d}() -->
  <Foo::{closure:%s:%d}>
x
  </Foo::{closure:%s:%d}>
  <Foo::{closure:%s:%d}>
x
  </Foo::{closure:%s:%d}>
</file '%s'>
