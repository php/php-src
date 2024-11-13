--TEST--
#[\Override]
--FILE--
<?php

interface I {
    public function i();
}

interface II extends I {
    #[\Override]
    public function i();
}

class P {
    public function p1() {}
    public function p2() {}
    public function p3() {}
    public function p4() {}
}

class PP extends P {
    #[\Override]
    public function p1() {}
    public function p2() {}
    #[\Override]
    public function p3() {}
}

class C extends PP implements I {
    #[\Override]
    public function i() {}
    #[\Override]
    public function p1() {}
    #[\Override]
    public function p2() {}
    public function p3() {}
    #[\Override]
    public function p4() {}
    public function c() {}
}

echo "Done";

?>
--EXPECT--
Done
