--TEST--
Specifying ticks through package declaration
--FILE--
<?php

register_tick_function(function() {
    echo "Tick\n";
});

package_declare([
    "name" => "with_ticks",
    "declares" => [
        "ticks" => 1,
    ],
]);

eval(<<<'PHP'
// Package defaults to ticks=1
package "with_ticks";

echo "Foo\n";
echo "Foo\n";
PHP);

eval(<<<'PHP'
// Manual override to ticks=0
package "with_ticks";
declare(ticks=0);

echo "Bar\n";
echo "Bar\n";
PHP);

eval(<<<'PHP'
// No package, no ticks

echo "Baz\n";
echo "Baz\n";
PHP);

?>
--EXPECT--
Foo
Tick
Foo
Tick
Bar
Bar
Baz
Baz
