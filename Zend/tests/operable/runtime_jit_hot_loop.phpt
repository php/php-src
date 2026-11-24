--TEST--
Operable dispatch under hot loops (OPcache and JIT safety)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=64M
--EXTENSIONS--
opcache
--FILE--
<?php

final readonly class Vec implements Operable {
    public function __construct(public int $x) {}

    public function __add(self|int $rhs): self {
        return new self($this->x + ($rhs instanceof self ? $rhs->x : $rhs));
    }

    public function __sub(int $rhs, OperandSide $side): self {
        return new self($side === OperandSide::Left ? $this->x - $rhs : $rhs - $this->x);
    }

    public function __mul(int $factor): self {
        return new self($this->x * $factor);
    }

    public function __div(int $divisor): string {
        return (string) intdiv($this->x, $divisor);
    }
}

function runHotLoop(): array {
    $acc = new Vec(0);
    $lastLabel = '';
    for ($i = 1; $i <= 500; $i++) {
        $v = new Vec($i);
        $scaled = 2 * $v;
        $diff = 1000 - $v;
        $acc += $scaled + $diff;
        $lastLabel = $acc / 10;
    }
    return [$acc->x, $lastLabel];
}

// Sum_{i=1..500} (2*i + (1000 - i)) = Sum_{i=1..500} (1000 + i) = 500000 + 125250 = 625250
var_dump(runHotLoop());

?>
--EXPECT--
array(2) {
  [0]=>
  int(625250)
  [1]=>
  string(5) "62525"
}
