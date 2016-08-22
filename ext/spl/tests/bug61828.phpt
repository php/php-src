--TEST--
Bug #61828 Directory(Recursive)Iterator/Spl(Temp)FileObject ctor twice cause memleak&segfault
--FILE--
<?php
/* Leak testing */
$x = new DirectoryIterator(".");
$x->__construct("..");

$x = new RecursiveDirectoryIterator(".");
$x->__construct("..");

$x = new FilesystemIterator(".");
$x->__construct("..");

$x = new GlobIterator(".");
$x->__construct("..");

$x = new SplFileObject(__FILE__);
$x->__construct(__FILE__);

$x = new SplTempFileObject(1);
$x->__construct(1);

echo "DONE testing normal double construct\n";

/*
 * When trying to get debug info by convert to string it will segfault.
 * So here we simply trigger it by type convertion
 */
$y = new DirectoryIterator(".");
try {
	$y->__construct("bug-path");
} catch (Exception $e)
{}
(string)$y;

$y = new RecursiveDirectoryIterator(".");
try {
	$y->__construct("bug-path");
} catch (Exception $e)
{}
(string)$y;

$y = new FilesystemIterator(".");
try {
	$y->__construct("bug-path");
} catch (Exception $e)
{}
(string)$y;

$y = new SplFileObject(__FILE__);
try {
	$y->__construct('bug-path');
} catch (Exception $e)
{}
(string)$y;

$y = new SplTempFileObject(1);
try {
	$y->__construct('bug-param');
} catch (Exception $e)
{}
(string)$y;

echo "DONE testing double construct with later one thrown exception\n";
?>
--EXPECT--
DONE testing normal double construct
DONE testing double construct with later one thrown exception