<?php

trait T1 {
    public function method() {
        // Needs to be optimized somehow.
        $str = "Foo";
        echo "$str\n";
    }
}

trait T2 {}

class C1 {
    use T1;
}

class C2 extends C1 {
    use T2;
}

trait T3 {
    public function method() {
        // Prevent trivial inheritance.
        static $x;
        // Needs to be optimized somehow.
        $str = "Foo";
        echo "$str\n";
    }
}
class C3 {
    use T3;
}
class C4 extends C3 {}
