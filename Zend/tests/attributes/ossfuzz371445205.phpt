--TEST--
OSS-Fuzz #371445205 (Heap-use-after-free in attr_free)
--FILE--
<?php
#[Attribute]
class MyAttrib{}
#[MyAttrib(notinterned:'')]
class Test1{}
$attr=(new ReflectionClass(Test1::class))->getAttributes()[0];
try {
    $attr->newInstance();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Unknown named parameter $notinterned
