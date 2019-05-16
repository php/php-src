--TEST--
Testing get_declared_classes() and get_declared_traits()
--FILE--
<?php

class a { }
interface b { }
trait c { }
abstract class d { }
final class e { }
var_dump(get_declared_classes());
var_dump(get_declared_traits());

?>
--EXPECTF--
array(%d) {%A
  [%d]=>
  string(1) "a"
  [%d]=>
  string(1) "d"
  [%d]=>
  string(1) "e"
}
array(%d) {%A
  [%d]=>
  string(1) "c"
}
