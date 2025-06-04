--TEST--
Readonly class cannot extend non-readonly class
--FILE--
<?php

class ParentClass {
    public int $prop;
}

readonly class Test extends ParentClass {
    public function __construct(
        public int $prop {
            get => $this->prop;
            set => $value;
        }
    ) {}
}

?>
--EXPECTF--
Fatal error: Readonly class Test cannot extend non-readonly class ParentClass in %s on line %d