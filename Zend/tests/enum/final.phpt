--TEST--
Enum is final
--FILE--
<?php

enum Foo {}

class Bar extends Foo {}

?>
--EXPECTF--
Fatal error: Class Bar cannot extend final class Foo in %s on line %d
