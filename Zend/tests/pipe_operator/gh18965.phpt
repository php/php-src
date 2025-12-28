--TEST--
GH-18965: ASSERT_CHECK avoids pipe lhs free
--INI--
zend.assertions=0
--FILE--
<?php
namespace Foo;
range(0, 10) |> assert(...);
echo "No leak\n";
?>
--EXPECT--
No leak
