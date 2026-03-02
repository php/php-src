--TEST--
GH-14009: Traits inherit prototype
--FILE--
<?php

class P {
    protected function common() {
        throw new Exception('Unreachable');
    }
}

class A extends P {
    public function test(P $sibling) {
        $sibling->common();
    }
}

class B extends P {
    protected function common() {
        echo __METHOD__, "\n";
    }
}

trait T {
    protected function common() {
        echo __METHOD__, "\n";
    }
}

class C extends P {
    use T;
}

$a = new A();
$a->test(new B());
$a->test(new C());

?>
--EXPECT--
B::common
T::common
