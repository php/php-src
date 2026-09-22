--TEST--
GH-19187: ReflectionNamedType::getName() should not include nullable type
--FILE--
<?php

class C {
    public string|null $a {
        set => $value;
    }
    public string|null $b {
        set(string|null $value) => $value;
    }
    public string|null $c {
        get => $this->c;
    }
}

foreach ((new ReflectionClass(C::class))->getProperties() as $r) {
    $type = $r->getType();
    echo $type, "\n";
    echo $type->getName(), "\n";
    var_dump($type->allowsNull());
    echo "\n";

    $settableType = $r->getSettableType();
    echo $settableType, "\n";
    echo $settableType->getName(), "\n";
    var_dump($settableType->allowsNull());
    echo "\n";
}

?>
--EXPECT--
?string
string
bool(true)

?string
string
bool(true)

?string
string
bool(true)

?string
string
bool(true)

?string
string
bool(true)

?string
string
bool(true)
