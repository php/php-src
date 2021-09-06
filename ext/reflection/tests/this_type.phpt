--TEST--
$this type reflection
--FILE--
<?php

class Test {
    public function method1(): $this {}
    public function method2(): ?$this {}
    public function method3(): $this|FooBar {}
}

function printType(ReflectionType $type) {
    echo (string) $type, "\n";
    if ($type instanceof ReflectionNamedType) {
        echo "  isBuiltin: "; var_dump($type->isBuiltin());
        echo "  getName: "; var_dump($type->getName());
    } else {
        foreach ($type->getTypes() as $type) {
            printType($type);
        }
    }
    echo "\n";
}

printType((new ReflectionMethod(Test::class, "method1"))->getReturnType());
printType((new ReflectionMethod(Test::class, "method2"))->getReturnType());
printType((new ReflectionMethod(Test::class, "method3"))->getReturnType());

?>
--EXPECT--
$this
  isBuiltin: bool(true)
  getName: string(5) "$this"

?$this
  isBuiltin: bool(true)
  getName: string(5) "$this"

FooBar|$this
FooBar
  isBuiltin: bool(false)
  getName: string(6) "FooBar"

static
  isBuiltin: bool(false)
  getName: string(6) "static"
