--TEST--
Abstract method in trait using "self"
--FILE--
<?php

trait T {
    abstract private function method(self $x): self;
}

class C {
    use T;

    private function method(self $x): self {
        return $this;
    }
}

?>
===DONE===
--EXPECT--
===DONE===
