--TEST--
Backed property cannot redeclare non-readonly as readonly property
--FILE--
<?php

class ParentClass {
    public int $prop;
}

class Test extends ParentClass {
    public function __construct(
        public readonly int $prop {
            get => $this->prop;
            set => $value;
        }
    ) {}
}

?>
--EXPECTF--
Fatal error: Cannot redeclare non-readonly property ParentClass::$prop as readonly Test::$prop in %s on line %d
