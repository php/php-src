--TEST--
ZE2 A private method cannot be called in a derived class
--FILE--
<?php

class pass {
    private static function show() {
        echo "Call show()\n";
    }

    public static function do_show() {
        pass::show();
    }
}

class fail extends pass {
    static function do_show() {
        pass::show();
    }
}

pass::do_show();
fail::do_show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Uncaught Error: Call to private method pass::show() from scope fail in %s:%d
Stack trace:
#0 %s(%d): fail::do_show()
#1 {main}
  thrown in %s on line %d
