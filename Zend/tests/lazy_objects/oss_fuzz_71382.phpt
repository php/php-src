--TEST--
oss-fuzz #71382
--FILE--
<?php

class C {
    public $a;
    public $b {
        get {
        }
    }
}

$reflector = new ReflectionClass(C::class);
$obj = $reflector->newLazyGhost(function() {
    throw new \Exception('initializer');
});

try {
    foreach($obj as $a) {
    }
} catch (Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

--EXPECT--
Exception: initializer
