--TEST--
Bug GH-9285: PHP 8.2 readonly classes allow inheriting mutable properties from traits - success
--FILE--
<?php

trait T {
    public readonly int $prop;
}

readonly class C {
    use T;

    public function __construct()
    {
        $this->prop = 1;
    }
}

$c = new C();
var_dump($c->prop);

?>
--EXPECT--
int(1)
