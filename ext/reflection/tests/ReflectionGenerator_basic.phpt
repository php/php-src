--TEST--
ReflectionGenerator basic test
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
    var_dump($gen);

    $gen->valid(); // start Generator
    $ref = new ReflectionGenerator($gen);

    var_dump($ref->getTrace());
    var_dump($ref->getExecutingLine());
    var_dump($ref->getExecutingFile());
    var_dump($ref->getExecutingGenerator());
    var_dump($ref->getFunction());
    var_dump($ref->getThis());
}

?>
--EXPECTF--
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "class@anonymous%s::a"
}
array(2) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(3) "foo"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(5) {
    ["function"]=>
    string(1) "a"
    ["class"]=>
    string(%d) "class@anonymous%s"
    ["object"]=>
    object(class@anonymous)#%d (0) {
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(0) {
    }
  }
}
int(%d)
string(%d) "%sReflectionGenerator_basic.%s"
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "foo"
}
object(ReflectionMethod)#8 (2) {
  ["name"]=>
  string(1) "a"
  ["class"]=>
  string(%d) "class@anonymous%s"
}
object(class@anonymous)#1 (0) {
}
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "{closure:%s:%d}"
}
array(1) {
  [0]=>
  array(2) {
    ["function"]=>
    string(%d) "{closure:%s:%d}"
    ["args"]=>
    array(0) {
    }
  }
}
int(%d)
string(%d) "%sReflectionGenerator_basic.%s"
object(Generator)#%d (1) {
  ["function"]=>
  string(%d) "{closure:%s:%d}"
}
object(ReflectionFunction)#7 (1) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
}
NULL
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "foo"
}
array(1) {
  [0]=>
  array(2) {
    ["function"]=>
    string(3) "foo"
    ["args"]=>
    array(0) {
    }
  }
}
int(%d)
string(%d) "%sReflectionGenerator_basic.%s"
object(Generator)#%d (1) {
  ["function"]=>
  string(3) "foo"
}
object(ReflectionFunction)#8 (1) {
  ["name"]=>
  string(3) "foo"
}
NULL
