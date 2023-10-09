--TEST--
GH-9447: Invalid class FQN emitted by AST dump for new and class constants in constant expressions
--FILE--
<?php

namespace App;

use SomewhereElse\Qux;

class Foo
{
    public function bar(
        $a = Bar::BAZ,
        $b = new Bar(),
        $c = new parent(),
        $d = new self(),
        $e = new namespace\Bar(),
        $f = new Qux(),
        $g = new namespace\Qux(),
        $i = new \Qux(),
    ) {}
}

$r = new \ReflectionMethod(Foo::class, 'bar');

foreach ($r->getParameters() as $p) {
    echo $p, "\n";
}

?>
--EXPECT--
Parameter #0 [ <optional> $a = \App\Bar::BAZ ]
Parameter #1 [ <optional> $b = new \App\Bar() ]
Parameter #2 [ <optional> $c = new parent() ]
Parameter #3 [ <optional> $d = new self() ]
Parameter #4 [ <optional> $e = new \App\Bar() ]
Parameter #5 [ <optional> $f = new \SomewhereElse\Qux() ]
Parameter #6 [ <optional> $g = new \App\Qux() ]
Parameter #7 [ <optional> $i = new \Qux() ]
