--TEST--
Enum allows constants
--FILE--
<?php

enum Foo {
    const BAR = 'Bar';
}

echo Foo::BAR . "\n";

?>
--EXPECT--
Bar
