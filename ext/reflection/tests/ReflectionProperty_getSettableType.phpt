--TEST--
ReflectionProperty::getSettableType()
--FILE--
<?php

class Test {
    public $plainUntyped;

    public int $plainTyped;

    public $virtualReadOnlyUntyped {
        get => 42;
    }

    public int $virtualReadOnlyTyped {
        get => 42;
    }

    public int $asymmetricVirtualTyped {
        get => 42;
        set(int|string $value) {}
    }

    public $backedUntyped {
        get => $this->backedUntyped;
        set => $value;
    }

    public int $backedTyped {
        get => $this->backedTyped;
        set => $value;
    }

    public int $asymmetricBackedTyped {
        get => $this->backedTyped;
        set(int|string $value) => (int) $value;
    }

    public int $backedTypedGetOnly {
        get => $this->backedTypedGetOnly;
    }
}

$reflectionClass = new ReflectionClass(Test::class);
foreach ($reflectionClass->getProperties() as $reflectionProperty) {
    $type = $reflectionProperty->getSettableType();
    echo ($type ? $type : "NULL") . "\n";
}

?>
--EXPECT--
NULL
int
never
never
string|int
NULL
int
string|int
int
