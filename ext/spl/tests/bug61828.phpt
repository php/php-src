--TEST--
Bug #61828 (Memleak when calling Directory(Recursive)Iterator/Spl(Temp)FileObject ctor twice)
--FILE--
<?php
$x = new DirectoryIterator('.');

try {
    $x->__construct('/tmp');
} catch (\Error $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
Directory object is already initialized
