--TEST--
ZE2 A protected method cannot be called in another class
--FILE--
<?php

class pass {
    protected static function show() {
        echo "Call pass::show()\n";
    }

    public static function do_show() {
        pass::show();
    }
}

pass::do_show();

class fail {
    public static function show() {
        echo "Call fail::show()\n";
        pass::show();
    }
}

fail::show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call pass::show()
Call fail::show()

Fatal error: Uncaught Error: Call to protected method pass::show() from scope fail in %s:%d
Stack trace:
#0 %s(%d): fail::show()
#1 {main}
  thrown in %s on line %d
