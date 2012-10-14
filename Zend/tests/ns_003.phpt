--TEST--
003: Name conflict (ns name)
--FILE--
<?php
namespace test\ns1;

class Exception {
}

echo get_class(new Exception()),"\n";
--EXPECT--
test\ns1\Exception
