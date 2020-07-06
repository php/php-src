--TEST--
ZE2 A private method can only be called inside the class
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

pass::do_show();
pass::show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Uncaught Error: Private method pass::show() cannot be called from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
