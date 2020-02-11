--TEST--
Basic decorator test
--FILE--
<?php

interface Component {
    public function getName(): string;
    public function getValue1(): int;
    public function getValue2(string $prefix): string;
}

class ConcreteBaseComponent implements Component {
    public function getName(): string {
        return "ConcreteBaseComponent";
    }
    public function getValue1(): int {
        return 42;
    }
    public function getValue2(string $prefix): string {
        return $prefix . "foo";
    }
}

class DecoratedComponent {
    private decorated Component $component;

    public function __construct(Component $component) {
        $this->component = $component;
    }
    public function getName(): string {
        return "DecoratedComponent({$this->component->getName()})";
    }
    public function getValue1(): int {
        return $this->component->getValue1() + 1;
    }
}

class ExtendDecoratedComponent extends DecoratedComponent {
    public function getValue1(): int {
        return parent::getValue1() + 1;
    }
    public function getValue2(string $prefix): string {
        return parent::getValue2($prefix) . "bar";
    }
}

$baseComponent = new ConcreteBaseComponent;
var_dump($baseComponent->getName());
var_dump($baseComponent->getValue1());
var_dump($baseComponent->getValue2("prefix: "));
$decorated = new DecoratedComponent($baseComponent);
var_dump($decorated->getName());
var_dump($decorated->getValue1());
var_dump($decorated->getValue2("prefix: "));
$decorated = new ExtendDecoratedComponent($baseComponent);
var_dump($decorated->getName());
var_dump($decorated->getValue1());
var_dump($decorated->getValue2("prefix: "));

?>
--EXPECT--
string(21) "ConcreteBaseComponent"
int(42)
string(11) "prefix: foo"
string(41) "DecoratedComponent(ConcreteBaseComponent)"
int(43)
string(11) "prefix: foo"
string(41) "DecoratedComponent(ConcreteBaseComponent)"
int(44)
string(14) "prefix: foobar"
