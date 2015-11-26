--TEST--
006: Run-time name conflict (ns name)
--FILE--
<?php
namespace test\ns1;

class Exception {
}

$x = "test\\ns1\\Exception";
echo get_class(new $x),"\n";
--EXPECT--
test\ns1\Exception
