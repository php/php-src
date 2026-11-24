--TEST--
ReflectionClass::getOperatorMethods() and ReflectionMethod::isOperatorMethod()
--FILE--
<?php

interface Addable extends Operable {
    public function __add(self $other): self;
}

trait MulTrait {
    public function __Mul(int $factor): self {
        return $this;
    }
}

abstract readonly class AbstractNum implements Addable {
    public function __construct(public int $v = 0) {}

    abstract public function __sub(int $rhs, OperandSide $side): self;

    final public function __pow(int $exp): self {
        return $this;
    }

    public function helper(): void {}
}

readonly class ConcreteNum extends AbstractNum {
    use MulTrait {
        __Mul as final;
    }

    // Declared out of symbol order and mixed case to verify deterministic order & case-insensitivity
    final public function __mod(int $m): self {
        return $this;
    }

    final public function __ADD(Addable $other): self {
        return $this;
    }

    final public function __sub(int $rhs, OperandSide $side): self {
        return $this;
    }

    public function __toString(): string {
        return (string) $this->v;
    }
}

final readonly class SubNum extends ConcreteNum {
    public function __div(int $d): self {
        return $this;
    }
}

class Plain {
    public function __add($other) {}
}

// 1. Concrete class with subset of operators + inherited + trait + mixed-case
$rcConcrete = new ReflectionClass(ConcreteNum::class);
$ops = $rcConcrete->getOperatorMethods();
var_dump(array_keys($ops));
foreach ($ops as $sym => $method) {
    echo "$sym => " . $method->getDeclaringClass()->getName() . "::" . $method->getName()
        . " op=" . ($method->isOperatorMethod() ? "true" : "false")
        . " final=" . ($method->isFinal() ? "true" : "false") . "\n";
}

// Non-operator methods on Operable class
var_dump($rcConcrete->getMethod('__construct')->isOperatorMethod());
var_dump($rcConcrete->getMethod('helper')->isOperatorMethod());
var_dump($rcConcrete->getMethod('__toString')->isOperatorMethod());

// 2. Subclass inheriting all 5 + adding '/' => all 6 in deterministic order
$rcSub = new ReflectionClass(SubNum::class);
$subOps = $rcSub->getOperatorMethods();
var_dump(array_keys($subOps));
var_dump($subOps['+']->isOperatorMethod());
var_dump($subOps['/']->isOperatorMethod());

// 3. ReflectionObject works identically
$ro = new ReflectionObject(new SubNum(10));
var_dump(array_keys($ro->getOperatorMethods()));

// 4. Interface extending Operable & Operable itself
$rcIface = new ReflectionClass(Addable::class);
var_dump(array_keys($rcIface->getOperatorMethods()));
var_dump($rcIface->getMethod('__add')->isOperatorMethod());
var_dump($rcIface->getMethod('__add')->isFinal());

$rcOperable = new ReflectionClass(Operable::class);
var_dump($rcOperable->getOperatorMethods());

// 5. Abstract readonly class (both abstract and concrete operator methods)
$rcAbs = new ReflectionClass(AbstractNum::class);
$absOps = $rcAbs->getOperatorMethods();
var_dump(array_keys($absOps));
var_dump($absOps['-']->isOperatorMethod(), $absOps['-']->isAbstract(), $absOps['-']->isFinal());
var_dump($absOps['**']->isOperatorMethod(), $absOps['**']->isAbstract(), $absOps['**']->isFinal());

// 6. Trait itself is not Operable
$rcTrait = new ReflectionClass(MulTrait::class);
var_dump($rcTrait->getOperatorMethods());
var_dump($rcTrait->getMethod('__Mul')->isOperatorMethod());

// 7. Non-Operable class defining __add
$rcPlain = new ReflectionClass(Plain::class);
var_dump($rcPlain->getOperatorMethods());
var_dump($rcPlain->getMethod('__add')->isOperatorMethod());

?>
--EXPECT--
array(5) {
  [0]=>
  string(1) "+"
  [1]=>
  string(1) "-"
  [2]=>
  string(1) "*"
  [3]=>
  string(1) "%"
  [4]=>
  string(2) "**"
}
+ => ConcreteNum::__ADD op=true final=true
- => ConcreteNum::__sub op=true final=true
* => ConcreteNum::__Mul op=true final=true
% => ConcreteNum::__mod op=true final=true
** => AbstractNum::__pow op=true final=true
bool(false)
bool(false)
bool(false)
array(6) {
  [0]=>
  string(1) "+"
  [1]=>
  string(1) "-"
  [2]=>
  string(1) "*"
  [3]=>
  string(1) "/"
  [4]=>
  string(1) "%"
  [5]=>
  string(2) "**"
}
bool(true)
bool(true)
array(6) {
  [0]=>
  string(1) "+"
  [1]=>
  string(1) "-"
  [2]=>
  string(1) "*"
  [3]=>
  string(1) "/"
  [4]=>
  string(1) "%"
  [5]=>
  string(2) "**"
}
array(1) {
  [0]=>
  string(1) "+"
}
bool(true)
bool(false)
array(0) {
}
array(3) {
  [0]=>
  string(1) "+"
  [1]=>
  string(1) "-"
  [2]=>
  string(2) "**"
}
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
array(0) {
}
bool(false)
array(0) {
}
bool(false)
