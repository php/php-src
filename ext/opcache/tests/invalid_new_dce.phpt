--TEST--
Throwings NEWs should not be DCEd
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

abstract class Foo {}
interface Bar {}
trait Baz {}

class Abc {
    const BAR = Abc::BAR;
}

function test1() {
    $x = new Foo;
}
function test2() {
    $x = new Bar;
}
function test3() {
    $x = new Baz;
}
function test4() {
    $x = new Abc;
}

try { test1(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { test2(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { test3(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { test4(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

?>
--EXPECT--
Error: Cannot instantiate abstract class Foo
Error: Cannot instantiate interface Bar
Error: Cannot instantiate trait Baz
Error: Cannot declare self-referencing constant Abc::BAR
