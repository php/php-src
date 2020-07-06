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
    private function f0() {}
}

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
Fatal error: Method fail::f0() must have public visibility to be compatible with overridden method same::f0() in %s on line %d
