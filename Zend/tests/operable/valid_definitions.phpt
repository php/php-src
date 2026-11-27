--TEST--
Valid Operable class, interface, trait, and inheritance definitions
--FILE--
<?php

namespace {
    interface MarkerA {}
    interface MarkerB {}

    interface Addable extends Operable {
        public function __add(self $other): self;
    }

    trait MulTrait {
        public function __mul(int|float $factor): self {
            return $this;
        }
    }

    abstract readonly class BaseNumber implements Addable {
        abstract public function __sub(int $other, OperandSide $side): static;

        final public function __pow(int|float $exp): int|float {
            return 1;
        }
    }

    final readonly class Number extends BaseNumber {
        use MulTrait;

        public function __add(Addable $other): self {
            return $this;
        }

        public function __sub(int $other, OperandSide $side): static {
            return $this;
        }

        public function __div((MarkerA&MarkerB)|int|float $rhs, OperandSide $side): ?self {
            return $this;
        }

        public function __mod(MarkerA&MarkerB $rhs): bool {
            return true;
        }

        // Non-operator magic-like methods are unaffected
        public function __custom($anything) {}
    }

    readonly class ExtensibleNumber implements Operable {
        use MulTrait {
            __mul as final;
        }

        final public function __add(int $x): self {
            return $this;
        }
    }

    $anon = new readonly class implements Operable {
        final public function __add(int $x): self {
            return $this;
        }
    };

    class NonOperable {
        public function __add($anything) {}
        private static function __sub() {}
    }

    var_dump(new Number() instanceof Operable);
    var_dump(new Number() instanceof Addable);
    var_dump(new ExtensibleNumber() instanceof Operable);
    var_dump($anon instanceof Operable);
    var_dump(new NonOperable() instanceof Operable);
}

namespace App {
    final readonly class NamespacedOp implements \Operable {
        public function __add(self $other, \OperandSide $side): self {
            return $this;
        }
    }

    var_dump(new NamespacedOp() instanceof \Operable);
    echo "OK\n";
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
OK
