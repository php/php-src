--TEST--
Enum is final
--FILE--
<?php

enum Foo {}

class Bar extends Foo {}

?>
--EXPECTF--
Fatal error: Class Bar may not inherit from final class (Foo) in %s on line %d
