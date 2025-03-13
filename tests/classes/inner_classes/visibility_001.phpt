--TEST--
outer class visibility
--FILE--
<?php

class Outer {
    private class Inner {}
    private class Other {
        public function __construct(private Outer:>Inner $inner) {}

        public function reset(): void {
            $prev = $this->inner;
            $this->inner = new Outer:>Inner();
            var_dump($prev === $this->inner);
        }
    }
    public static function test(): void {
        new self:>Other(new Outer:>Inner());
    }
}
Outer::test();
?>
--EXPECT--
