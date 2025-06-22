--TEST--
Backed property cannot redeclare readonly as non-readonly property
--FILE--
<?php

class ParentClass {
    public readonly int $prop;
}

class Test extends ParentClass {
    public function __construct(
        public int $prop {
            get => $this->prop;
            set => $value;
        }
    ) {}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare readonly property ParentClass::$prop as non-readonly Test::$prop in %s on line %d
