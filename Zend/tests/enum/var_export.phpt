--TEST--
Enum var_export
--FILE--
<?php

enum Foo {
    case Bar;
}

var_export(Foo::Bar);
// Should not warn about recursion
echo "\n";
echo str_replace(" \n", "\n", var_export([Foo::Bar], true));

?>
--EXPECT--
\Foo::Bar
array (
  0 =>
  \Foo::Bar,
)
