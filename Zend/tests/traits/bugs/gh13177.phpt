--TEST--
GH-13177 (PHP 8.3.2: final private constructor not allowed when used in trait)
--FILE--
<?php

trait Bar {
    final private function __construct() {}
}

final class Foo1 {
    use Bar;
}

final class Foo2 {
    use Bar {
        __construct as final;
    }
}

class Foo3 {
    use Bar {
        __construct as final;
    }
}

trait TraitNonConstructor {
    private final function test() {}
}

class Foo4 {
    use TraitNonConstructor { test as __construct; }
}

for ($i = 1; $i <= 4; $i++) {
    $rc = new ReflectionClass("Foo$i");
    echo $rc->getMethod("__construct"), "\n";
}

class Foo5 extends Foo3 {
    private function __construct() {}
}

?>
--EXPECTF--
Warning: Private methods cannot be final as they are never overridden by other classes in %s on line %d
Method [ <user, ctor> final private method __construct ] {
  @@ %sgh13177.php 4 - 4
}

Method [ <user, ctor> final private method __construct ] {
  @@ %sgh13177.php 4 - 4
}

Method [ <user, ctor> final private method __construct ] {
  @@ %sgh13177.php 4 - 4
}

Method [ <user, ctor> final private method __construct ] {
  @@ %sgh13177.php 24 - 24
}


Fatal error: Cannot override final method Foo3::__construct() in %s on line %d
