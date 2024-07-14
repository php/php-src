--TEST--
Generated hooks in property promotion
--FILE--
<?php

class Test {
    public function __construct(
        public $prop {
            get { echo "get\n"; }
            set { echo "set($value)\n"; }
        },
    ) {}
}

$test = new Test(42);
echo $test->prop;

?>
--EXPECT--
set(42)
get
