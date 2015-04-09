--TEST--
Bug #43323 (Wrong count abstract methods)
--FILE--
<?php
abstract class bar {
    abstract public function bar();
}

class foo extends bar {
}
--EXPECTF--
Fatal error: Class foo contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (bar::bar) in %sbug43323.php on line 7
