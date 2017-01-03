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
--EXPECTF--
array(%d) {%A
  [%d]=>
  string(1) "c"
}
