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

echo "Exception in __debugInfo:\n";

try {
    print_r(new A());
} catch (\Throwable $e) {
    echo $e . "\n";
}

echo "\nException in __debugInfo in anonymous class:\n";

try {
    print_r(new class() extends A {
        public function __debugInfo(): array
        {
            throw new \Exception('y');
        }
    });
} catch (\Throwable $e) {
    echo $e . "\n";
}

echo "\nException in __destruct of __debugInfo in anonymous class:\n";

try {
    print_r(new class() extends A {
        public function __debugInfo(): array
        {
            $o = new class() {
                public function __destruct()
                {
                    throw new \Exception('z_w_assign');
                }
            };

            return ['foo' => 2];
        }
    });
} catch (\Throwable $e) {
    echo $e . "\n";
}

echo "\nException occuring later on:\n";

try {
    print_r(['foo' => 'bar', 'baz' => new A()]);
} catch (\Throwable $e) {
    echo $e . "\n";
}

?>
--EXPECTF--
Exception in __debugInfo:
A Object
Error: x in %s:%d
Stack trace:
#0 [internal function]: A->__debugInfo()
#1 %s(%d): print_r(Object(A))
#2 {main}

Exception in __debugInfo in anonymous class:
A@anonymous Object
Exception: y in %s:%d
Stack trace:
#0 [internal function]: A@anonymous->__debugInfo()
#1 %s(%d): print_r(Object(A@anonymous))
#2 {main}

Exception in __destruct of __debugInfo in anonymous class:
A@anonymous Object
Exception: z_w_assign in %s:%d
Stack trace:
#0 [internal function]: class@anonymous->__destruct()
#1 %s(%d): print_r(Object(A@anonymous))
#2 {main}

Exception occuring later on:
Array
(
    [foo] => bar
    [baz] => A Object

)
Error: x in %s:%d
Stack trace:
#0 [internal function]: A->__debugInfo()
#1 %s(%d): print_r(Array)
#2 {main}
