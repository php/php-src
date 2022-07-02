--TEST--
007: Run-time name conflict (php name)
--FILE--
<?php
namespace test\ns1;

class Exception {
}

$x = "Exception";
echo get_class(new $x),"\n";
?>
--EXPECT--
Exception
