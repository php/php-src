--TEST--
Bug #74922 (Composed class has fatal error with duplicate, equal const properties)
--FILE--
<?php

const VALUE = true;

trait Foo {public $var = VALUE;}
trait Bar {public $var = true;}
class Baz {use Foo, Bar;}

echo "DONE";

?>
--EXPECT--
DONE
