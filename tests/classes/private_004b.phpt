--TEST--
ZE2 A private method cannot be called in a derived class
--FILE--
<?php

class pass {
    private function show() {
        echo "Call show()\n";
    }

    public function do_show() {
        $this->show();
    }
}

class fail extends pass {
    function do_show() {
        $this->show();
    }
}

$t = new pass();
$t->do_show();

$t2 = new fail();
$t2->do_show();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Uncaught Error: Call to private method pass::show() from scope fail in %s:%d
Stack trace:
#0 %s(%d): fail->do_show()
#1 {main}
  thrown in %s on line %d
