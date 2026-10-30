--TEST--
Interface delegation: return by reference
--FILE--

<?php

interface A {
    public function &get(): mixed;
}

class AImpl implements A {
    private mixed $value = 'before';

    public function &get(): mixed {
        return $this->value;
    }
}

class C implements A by $a {
    public function __construct(protected A $a) {}
}

$c = new C(new AImpl);

$value =& $c->get();
$value = 'after';

echo $c->get(), "\n";
?>

--EXPECT--
after
