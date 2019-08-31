--TEST--
Throwings NEWs should not be DCEd
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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

try { test1(); } catch (Error $e) { echo $e->getMessage(), "\n"; }
try { test2(); } catch (Error $e) { echo $e->getMessage(), "\n"; }
try { test3(); } catch (Error $e) { echo $e->getMessage(), "\n"; }
try { test4(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

?>
--EXPECT--
Cannot instantiate abstract class Foo
Cannot instantiate interface Bar
Cannot instantiate trait Baz
Cannot declare self-referencing constant 'Abc::BAR'
