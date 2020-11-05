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
