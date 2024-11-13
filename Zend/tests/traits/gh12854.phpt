--TEST--
GH-12854 (8.3 - as final trait-used method does not correctly report visibility in Reflection)
--FILE--
<?php

trait SimpleTrait
{
    public function pub() {}
    protected function prot() {}
    private function priv() {}

    public final function final1() {}
    public final function final2() {}
    public final function final3() {}
}


class Test
{
    use SimpleTrait {
        pub as final;
        prot as final;
        priv as final;

        final1 as private;
        final2 as protected;
        final3 as public;
    }
}

foreach (['pub', 'prot', 'priv', 'final1', 'final2', 'final3'] as $method) {
    echo "--- Method: $method ---\n";
    $rm = new ReflectionMethod(Test::class, $method);
    var_dump($rm->isFinal());
    var_dump($rm->isPublic());
    var_dump($rm->isProtected());
    var_dump($rm->isPrivate());
}

?>
--EXPECTF--
Warning: Private methods cannot be final as they are never overridden by other classes in %s on line %d

Warning: Private methods cannot be final as they are never overridden by other classes in %s on line %d
--- Method: pub ---
bool(true)
bool(true)
bool(false)
bool(false)
--- Method: prot ---
bool(true)
bool(false)
bool(true)
bool(false)
--- Method: priv ---
bool(true)
bool(false)
bool(false)
bool(true)
--- Method: final1 ---
bool(true)
bool(false)
bool(false)
bool(true)
--- Method: final2 ---
bool(true)
bool(false)
bool(true)
bool(false)
--- Method: final3 ---
bool(true)
bool(true)
bool(false)
bool(false)
