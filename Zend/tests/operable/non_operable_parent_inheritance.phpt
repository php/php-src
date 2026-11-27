--TEST--
Non-Operable parent inheritance, final class exemption, reflection context sensitivity, and abstract/interface chains
--FILE--
<?php

// 1. Final operator method on non-Operable Base inherited by non-final Operable Child
readonly class Base {
    public function __construct(public int $v = 0) {}

    final public function __add(int $x): static {
        return new static($this->v + $x);
    }
}

readonly class Child extends Base implements Operable {
}

$anonFromFinalBase = new readonly class(10) extends Base implements Operable {
};

var_dump((new Child(10) + 5)->v);
var_dump(($anonFromFinalBase + 5)->v);

// Reflection depends on reflected class (Child vs Base)
$rmChild = new ReflectionMethod(Child::class, '__add');
$rmBase = new ReflectionMethod(Base::class, '__add');
var_dump($rmChild->isOperatorMethod());
var_dump($rmChild->isFinal());
var_dump($rmBase->isOperatorMethod());
var_dump($rmBase->isFinal());

var_dump((new ReflectionClass(Base::class))->getOperatorMethods());
$childOps = (new ReflectionClass(Child::class))->getOperatorMethods();
var_dump(array_keys($childOps));
var_dump($childOps['+']->isOperatorMethod());
var_dump($childOps['+']->isFinal());
var_dump($childOps['+']->getDeclaringClass()->getName());

// 2. Non-final Base stays non-final for non-Operable siblings;
//    final Child can inherit directly, and non-final Child can delegate with explicit final
readonly class NonFinalBase {
    public function __construct(public int $v = 0) {}

    public function __add(int $x): static {
        return new static($this->v + $x);
    }
}

final readonly class FinalChild extends NonFinalBase implements Operable {
}

readonly class DelegatingChild extends NonFinalBase implements Operable {
    final public function __add(int $x): static {
        return parent::__add($x);
    }
}

readonly class Other extends NonFinalBase {
    public function __add(int $x): static {
        return new static($this->v + $x * 2);
    }
}

var_dump((new ReflectionMethod(NonFinalBase::class, '__add'))->isFinal());
var_dump((new ReflectionMethod(FinalChild::class, '__add'))->isFinal());
var_dump((new ReflectionMethod(FinalChild::class, '__add'))->isOperatorMethod());
var_dump((new ReflectionMethod(DelegatingChild::class, '__add'))->isFinal());
var_dump((new FinalChild(10) + 5)->v);
var_dump((new DelegatingChild(10) + 5)->v);
var_dump((new Other(10))->__add(5)->v);

// 3. Abstract non-Operable parent and non-Operable interface chains
interface PlainAdd {
    public function __add(int $x): self;
}

interface OperableAdd extends PlainAdd, Operable {
}

abstract readonly class AbstractBase {
    abstract public function __sub(int $x, OperandSide $side): self;
}

abstract readonly class AbstractChild extends AbstractBase implements OperableAdd {
}

final readonly class ConcreteGrand extends AbstractChild {
    public function __add(int $x): self {
        return $this;
    }

    public function __sub(int $x, OperandSide $side): self {
        return $this;
    }
}

$rcAbsChild = new ReflectionClass(AbstractChild::class);
$absOps = $rcAbsChild->getOperatorMethods();
var_dump(array_keys($absOps));
var_dump($absOps['+']->isOperatorMethod(), $absOps['+']->isAbstract(), $absOps['+']->isFinal());
var_dump($absOps['-']->isOperatorMethod(), $absOps['-']->isAbstract(), $absOps['-']->isFinal());
var_dump((new ReflectionMethod(PlainAdd::class, '__add'))->isOperatorMethod());
var_dump((new ReflectionMethod(AbstractBase::class, '__sub'))->isOperatorMethod());

$rcGrand = new ReflectionClass(ConcreteGrand::class);
$grandOps = $rcGrand->getOperatorMethods();
var_dump($rcGrand->isFinal());
var_dump($grandOps['+']->isOperatorMethod(), $grandOps['+']->isFinal());
var_dump($grandOps['-']->isOperatorMethod(), $grandOps['-']->isFinal());

?>
--EXPECT--
int(15)
int(15)
bool(true)
bool(true)
bool(false)
bool(true)
array(0) {
}
array(1) {
  [0]=>
  string(1) "+"
}
bool(true)
bool(true)
string(4) "Base"
bool(false)
bool(false)
bool(true)
bool(true)
int(15)
int(15)
int(20)
array(2) {
  [0]=>
  string(1) "+"
  [1]=>
  string(1) "-"
}
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
