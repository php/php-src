--TEST--
Generators expose the underlying function name in __debugInfo().
--FILE--
<?php

function foo() {
    yield;
}

$gens = [
    (new class() {
        function a() {
            yield from foo();
        }
    })->a(),
    (function() {
        yield;
    })(),
    foo(),
];

foreach ($gens as $gen) {
    echo "Before:", PHP_EOL;
    var_dump($gen);

    foreach ($gen as $dummy) {
        echo "Inside:", PHP_EOL;
        var_dump($gen);
    }

    echo "After:", PHP_EOL;

    var_dump($gen);
}

?>
--EXPECTF--
Before:
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "class@anonymous%s::a"
}
Inside:
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "class@anonymous%s::a"
}
After:
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "class@anonymous%s::a"
}
Before:
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "{closure:%s:%d}"
}
Inside:
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "{closure:%s:%d}"
}
After:
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "{closure:%s:%d}"
}
Before:
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "foo"
}
Inside:
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "foo"
}
After:
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "foo"
}
