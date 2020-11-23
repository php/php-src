--TEST--
ZE2 A redeclared method must have the same or higher visibility
--FILE--
<?php

class father {
    function f0() {}
    function f1() {}
    public function f2() {}
    protected function f3() {}
    private function f4() {}
}

class same extends father {

    // overload fn with same visibility
    function f0() {}
    public function f1() {}
    public function f2() {}
    protected function f3() {}
    private function f4() {}
}

class fail extends same {
    private function f1() {}
}

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Fatal error: Access level to fail::f1() must be public (as in class same) in %s on line 22
