--TEST--
ReflectionGenerator while being currently executed
--FILE--
<?php

function call(ReflectionGenerator $ref, $method, $rec = true) {
    if ($rec) {
        call($ref, $method, false);
        return;
    }
    var_dump($ref->$method());
}

function doCalls(ReflectionGenerator $ref) {
    call($ref, "getTrace");
    call($ref, "getExecutingLine");
    call($ref, "getExecutingFile");
    call($ref, "getExecutingGenerator");
    call($ref, "getFunction");
    call($ref, "getThis");
}

($gen = (function() use (&$gen) {
    $ref = new ReflectionGenerator($gen);

    doCalls($ref);

    yield from (function() use ($ref) {
        doCalls($ref);
        yield; // Generator !
    })();
})())->next();

?>
--EXPECTF--
array(0) {
}
int(%d)
string(%d) "%sReflectionGenerator_in_Generator.%s"
object(Generator)#2 (0) {
}
object(ReflectionFunction)#4 (1) {
  ["name"]=>
  string(9) "{closure}"
}
NULL
array(1) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(9) "{closure}"
    ["args"]=>
    array(0) {
    }
  }
}
int(%d)
string(%d) "%sReflectionGenerator_in_Generator.%s"
object(Generator)#5 (0) {
}
object(ReflectionFunction)#6 (1) {
  ["name"]=>
  string(9) "{closure}"
}
NULL
