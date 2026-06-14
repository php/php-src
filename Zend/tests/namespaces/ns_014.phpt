--TEST--
014: Name conflict and functions (php name)
--FILE--
<?php
namespace test\ns1;

echo strlen("Hello"),"\n";
?>
--EXPECT--
5
