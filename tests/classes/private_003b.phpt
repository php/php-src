--TEST--
ZE2 A private method cannot be called in a derived class
--FILE--
<?php

class pass {
    private function show() {
        echo "Call show()\n";
    }

    protected function good() {
        $this->show();
    }
}

class fail extends pass {
    public function ok() {
        $this->good();
    }

    public function not_ok() {
        $this->show();
    }
}

$t = new fail();
$t->ok();
$t->not_ok(); // calling a private function

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Call show()

Fatal error: Uncaught Error: Call to private method pass::show() from scope fail in %s:%d
Stack trace:
#0 %s(%d): fail->not_ok()
#1 {main}
  thrown in %s on line %d
