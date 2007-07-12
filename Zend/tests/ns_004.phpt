--TEST--
004: Name conflict (php name)
--FILE--
<?php
namespace test::ns1;

echo get_class(new Exception()),"\n";
--EXPECT--
Exception
