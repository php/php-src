--TEST--
Bug #41372 (Internal pointer of source array resets during array copying)
--FILE--
<?php
$Foo = array('val1', 'val2', 'val3');
end($Foo);
echo key($Foo),"\n";
$MagicInternalPointerResetter = $Foo;
echo key($Foo),"\n";
?>
--EXPECT--
2
2
