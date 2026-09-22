--TEST--
Bug #71248 (Wrong interface is enforced)
--FILE--
<?php
class Hint1 { }
class Hint2 { }

abstract class Base {
    public function __construct(Hint1 $x) { }
}

interface Iface {
    public function __construct(Hint1 $x);
}

class Foo extends Base implements Iface {
}

$code = <<<'PHP'
abstract class Bar extends Base {
    public function __construct(Hint2 $x) { }
}
PHP;
eval($code);
?>
OK
--EXPECT--
OK
