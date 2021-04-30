--TEST--
Complex accessors in promotion not allowed
--FILE--
<?php

class Test {
    public function __construct(
        public $prop {
            get { echo "get\n"; }
            set { echo "set\n"; }
        },
    ) {}
}

?>
--EXPECTF--
Fatal error: Can only use implicit accessors for a promoted property in %s on line %d
