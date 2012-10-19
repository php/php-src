--TEST--
004: Using global class name from namespace (unqualified - fail)
--FILE--
<?php
namespace test\ns1;

echo get_class(new Exception()),"\n";
--EXPECTF--
Fatal error: Class 'test\ns1\Exception' not found in %sns_004.php on line %d