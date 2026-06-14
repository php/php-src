--TEST--
Bug #80055: Abstract trait methods returning "self" cannot be fulfilled by traits
--FILE--
<?php

trait AbstractTrait {
    abstract public function selfReturner(): self;
}

trait ConcreteTrait {
    public function selfReturner(): self {
        return $this;
    }
}

class Test {
    use AbstractTrait;
    use ConcreteTrait;
}

?>
===DONE===
--EXPECT--
===DONE===
