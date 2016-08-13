--TEST--
Bug #71414 (Interface method override inherited method and implemented in a trait causes fatal error)
--FILE--
<?php
interface InterfaceY {
    public function z(): string;
}

trait TraitY {
    public function z(): string {
    }
}

class X {
    public function z() {
    }
}

class Y extends X implements InterfaceY {
    use TraitY;
}

echo "ok";
--EXPECT--
ok
