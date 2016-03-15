--TEST--
Bug #71825 (PHP does not allow weaker parameter signature in child interface/class)
--FILE--
<?php

interface I {

  function fooMethod(J $j);

}

interface J extends I {

  function fooMethod(I $i);

}

class Foo {

  function fooMethod(J $j) {}

}

class Bar extends Foo {

  function fooMethod(I $i) {}

}

class Baz implements I {

  function fooMethod(I $i) {}

}

echo "Done\n";

?>
--EXPECT--
Done
