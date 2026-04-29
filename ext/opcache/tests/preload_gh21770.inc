<?php
trait A {
    public ?string $a = 'a' {
        get => $this->a;
    }
}

trait X {
    public int $x = 0 {
        set(int $value) => $this->x = $value;
    }
}

class B {
    use A;
}

class C {
    use X;
}
