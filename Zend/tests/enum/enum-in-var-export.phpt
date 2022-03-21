--TEST--
Enum in var_export()
--FILE--
<?php

namespace {
    enum Foo { case BAR; }
}

namespace A {
    enum Foo { case BAR; }
}

namespace A\B {
    enum Foo { case BAR; }
}

namespace Test {
    echo var_export(\Foo::BAR, true) . "\n";
    echo var_export(\A\Foo::BAR, true) . "\n";
    echo var_export(\A\B\Foo::BAR, true) . "\n";
}

?>
--EXPECT--
\Foo::BAR
\A\Foo::BAR
\A\B\Foo::BAR
