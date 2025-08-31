--TEST--
005: Name conflict (php name in case if ns name exists)
--FILE--
<?php
namespace test\ns1;

class Exception {
}

echo get_class(new \Exception()),"\n";
?>
--EXPECT--
Exception
