--TEST--
GH-22399 (Lazy object revert init double free, dynamic property variant)
--FILE--
<?php

#[AllowDynamicProperties]
class A {
    public $trigger;
}

class B {
    public function __construct(public $a) {}
    public function __destruct() {
        unset($this->a->dyn);
    }
}

$reflector = new ReflectionClass(A::class);

$a = $reflector->newLazyGhost(function (A $a) {
    $a->dyn = new B($a);
    throw new Exception('initializer exception');
});

try {
    $a->trigger;
} catch (Exception $e) {
    printf("%s\n", $e->getMessage());
}

var_dump($a);
printf("Is lazy: %d\n", $reflector->isUninitializedLazyObject($a));
echo "done\n";
?>
--EXPECTF--
initializer exception
lazy ghost object(A)#%d (0) {
}
Is lazy: 1
done
