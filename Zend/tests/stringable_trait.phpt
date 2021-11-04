--TEST--
Bug #81582: Stringable not implicitly declared if __toString() came from a trait
--FILE--
<?php

trait T {
    public function __toString(): string {
        return "ok";
    }
}

class C {
    use T;
}

var_dump(new C instanceof Stringable);

?>
--EXPECT--
bool(true)
