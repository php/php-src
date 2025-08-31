--TEST--
ZE2 A protected method can only be called inside the class
--FILE--
<?php

class pass {
    protected static function fail() {
        echo "Call fail()\n";
    }

    public static function good() {
        pass::fail();
    }
}

pass::good();
pass::fail();// must fail because we are calling from outside of class pass

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call fail()

Fatal error: Uncaught Error: Call to protected method pass::fail() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
