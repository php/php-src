--TEST--
Bug #79867: Promoted untyped properties should get null default value
--FILE--
<?php

class A {
    public function __construct(
        public $untyped = 1,
        public int $typed = 2,
    ) {}
}

class B extends A {
    public function __construct() {
        // Missing parent::__construct() call,
        // properties will not be initialized.
    }
}

var_dump(new B);

?>
--EXPECT--
object(B)#1 (1) {
  ["untyped"]=>
  NULL
  ["typed"]=>
  uninitialized(int)
}
