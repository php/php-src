--TEST--
Bug #61828 (Memleak when calling Directory(Recursive)Iterator/Spl(Temp)FileObject ctor twice)
--FILE--
<?php
$x = new DirectoryIterator('.');
$x->__construct('/tmp');
echo "Okey";
?>
--EXPECTF--
Warning: DirectoryIterator::__construct(): Directory object is already initialized in %sbug61828.php on line 3
Okey
