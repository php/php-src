--TEST--
GH-10169: Fix use-after-free when releasing object during property assignment
--FILE--
<?php
class A
{
    public string $prop;
}
class B
{
    public function __toString()
    {
        global $a;
        $a = null;
        return str_repeat('a', 1);
    }
}

$a = new A();
try {
    $a->prop = new B();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$a = new A();
$a->prop = '';
try {
    $a->prop = new B();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Object was released while assigning to property A::$prop
Error: Object was released while assigning to property A::$prop
