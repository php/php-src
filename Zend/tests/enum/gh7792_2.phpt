--TEST--
GH-7792 (Refer to enum as enum instead of class)
--FILE--
<?php

enum Foo implements Throwable {}

?>
--EXPECTF--
Fatal error: Enum Foo cannot implement interface Throwable in %s on line %d
