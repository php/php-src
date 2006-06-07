--TEST--
Bug #37707 (clone without assigning leaks memory)
--FILE--
<?php
class testme {}
clone new testme();
echo "NO LEAK\n";
?>
--EXPECT--
NO LEAK

