--TEST--
Testing get_declared_traits()
--FILE--
<?php

class a { }
interface b { }
trait c { }
abstract class d { }
final class e { }

var_dump(get_declared_traits());

?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "c"
}
