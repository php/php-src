--TEST--
GH-14156 (Abstract trait requirement replaces inherited private method)
--FILE--
<?php
trait T {
    public abstract function test(): void;

    public function run(): void {
        $this->test();
    }
}

class P {
    private function test(): void {}
}

abstract class C extends P {
    use T;
}

$method = new ReflectionMethod(C::class, 'test');
var_dump($method->isAbstract());
var_dump($method->getDeclaringClass()->getName());

class D extends C {
    public function test(): void {
        echo "implemented\n";
    }
}

(new D())->run();
?>
--EXPECT--
bool(true)
string(1) "C"
implemented
