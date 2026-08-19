--TEST--
Use array append as initialization
--FILE--
<?php

class C {
    public readonly array $a;

    public function init() {
        $this->a[] = 1;
        var_dump($this->a);
    }
}


function init() {
    $c = new C;
    $c->a[] = 1;
    var_dump($c->a);
}

try {
    (new C)->init();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    init();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot indirectly modify readonly property C::$a
Error: Cannot indirectly modify readonly property C::$a
