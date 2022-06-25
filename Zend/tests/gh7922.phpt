--TEST--
GH-7922 (print_r should not produce a fatal error)
--FILE--
<?php
class A
{
    public function __debugInfo()
    {
        throw new \Error('x');
    }
}

try {
    $a = new A();
    print_r($a);
} catch (\Throwable $e) {
    echo (string) $e . "\n";
}

try {
    $a = new class() extends A {
        public function __debugInfo(): array
        {
            throw new \Exception('y');
        }
    };
    print_r($a, true);
} catch (\Throwable $e) {
    echo (string) $e . "\n";
}
?>
--EXPECTF--
Error: x in %s:6
Stack trace:
#0 [internal function]: A->__debugInfo()
#1 %s(12): print_r(Object(A))
#2 {main}

Exception: y in %s:21
Stack trace:
#0 [internal function]: A@anonymous->__debugInfo()
#1 %s(24): print_r(Object(A@anonymous), true)
#2 {main}
