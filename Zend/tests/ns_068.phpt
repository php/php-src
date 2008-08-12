--TEST--
068: Code before namespace
--FILE--
<?php
echo __NAMESPACE__ . "\n";
namespace foo;
echo __NAMESPACE__ . "\n";
namespace bar;
echo __NAMESPACE__ . "\n";
?>
===DONE===
--EXPECTF--

Fatal error: Namespace declaration statement has to be the very first statement in the script in %sns_068.php on line %d
