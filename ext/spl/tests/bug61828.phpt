--TEST--
Testing Directory(Recursive)Iterator/Spl(Temp)FileObject ctor twice
--FILE--
<?php
$x = new DirectoryIterator(".");
$x->__construct("..");

$x = new RecursiveDirectoryIterator(".");
$x->__construct("..");

$x = new FilesystemIterator(".");
$x->__construct("..");

$x = new GlobIterator(".");
$x->__construct("..");

$x = new SplFileObject('.');
$x->__construct('..');

$x = new SplTempFileObject(1);
$x->__construct(1);

echo "done!\n";
?>
--EXPECT--
done!