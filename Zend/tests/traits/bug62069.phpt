--TEST--
Bug #62069: binding wrong traits if they have same name methods
--FILE--
<?php

trait T1 {
    public function func() {
        echo "From T1\n";
    }
}

trait T2 {
    public function func() {
        echo "From T2\n";
    }
}

class Bar {
    public function func() {
        echo "From Bar\n";
    }
    use T1, T2 {
        func as f1;
    }
}

$b = new Bar();
$b->f2();

?>
--EXPECTF--
Fatal error: An alias was defined for method func(), which exists in both T1 and T2. Use T1::func or T2::func to resolve the ambiguity in %s on line %d
