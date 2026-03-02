--TEST--
Backed enums can contain magic constants
--FILE--
<?php

enum Foo: string {
    case Bar = __FILE__;
}

echo Foo::Bar->value, "\n";

?>
--EXPECTF--
%smagic-constants.php
