--TEST--
Anonymous Operable class must declare concrete operator methods final (blocks class_alias override)
--FILE--
<?php

$a = new readonly class implements Operable {
    public function __add(int $x): self {
        return $this;
    }
};

class_alias(get_class($a), 'SneakyBase');
eval('readonly class Evil extends SneakyBase {}');
eval('final readonly class EvilGrand extends Evil {
    public function __add(int $x): self {
        return $this;
    }
}');

?>
--EXPECTF--
Fatal error: Operator method Operable@anonymous::__add() on non-final class must be declared final in %s on line 3
