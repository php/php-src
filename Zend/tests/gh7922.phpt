--TEST--
GH-7922 (print_r should not produce a fatal error)
--FILE--
<?php
class A
{
    public function __debugInfo(): array
    {
        throw new \Error('x');
    }
}

try {
    $a = new A();
    print_r($a, true);
} catch (\Throwable $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
__debuginfo() must return an array
