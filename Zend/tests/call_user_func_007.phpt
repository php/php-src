--TEST--
Testing scope overwriting with call_user_fun() in dynamic context
This test was written by "smalyshev" in this tread https://github.com/php/php-src/pull/375#issuecomment-20945580
--FILE--
<?php

class A {
  function foo() {
    echo "I am A\n";
  }
}

class B extends A {
  function foo() {
    echo "I am B\n";
  }
  function who() {
        var_dump(get_class($this));
        call_user_func(array($this, "foo"));
  }
}

class C extends B {
  function foo() {
    echo "I am C\n";
  }

  function who() {
        var_dump(get_class($this));
        call_user_func(array($this, "foo"));
        parent::who();
  }
}

$c = new C;
$c->who();
?>
--EXPECTF--
string(1) "C"
I am C
string(1) "C"
I am C