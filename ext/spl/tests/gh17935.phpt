--TEST--
GH-17935 (ArrayObject __serialize() should not cause assertion failure on modification)
--FILE--
<?php
$o = new ArrayObject();
$s2 = $o->__serialize();
$o['a'] = 'b';
echo "Success\n";
?>
--EXPECT--
Success
