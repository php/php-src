--TEST--
053: Run-time constant definition
--FILE--
<?php
namespace test\ns1;

define(__NAMESPACE__ . '\\NAME', basename(__FILE__));
echo NAME."\n";
echo \test\ns1\NAME."\n";
?>
--EXPECT--
ns_053.php
ns_053.php
