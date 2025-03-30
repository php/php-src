--TEST--
returning private class via interface
--FILE--
<?php
interface test {
    static function foo(): self;
}

class Outer {
  private class Middle {
    public class Inner implements test {
      public static function foo(): self {
        return new self();
      }
    }
  }
  private function foo(Middle\Inner $inner) {} // we can see Middle\Inner here
  public function test(): test {
    $this->foo($return = Middle\Inner::foo());
    return $return;
  }
}

var_dump(new Outer()->test());
var_dump(new Outer\Middle\Inner());
?>
--EXPECTF--
object(Outer\Middle\Inner)#2 (0) {
}

Fatal error: Uncaught TypeError: Cannot instantiate class Outer\Middle\Inner from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
