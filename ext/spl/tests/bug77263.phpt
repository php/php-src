--TEST--
Bug #77263 (Segfault when using 2 RecursiveFilterIterator)
--FILE--
<?php
$dir = __DIR__ . '/bug77263';
mkdir($dir);
mkdir("$dir/subdir");
touch("$dir/file1");
touch("$dir/subdir/file2");

class Filter1 extends RecursiveFilterIterator {
    public function accept(): bool { return $this->getInnerIterator()->getSubPathname() != ''; }
}

class Filter2 extends RecursiveFilterIterator {
    public function accept(): bool { return $this->getInnerIterator()->getSubPathname() != ' '; }
}

$iterator = new RecursiveDirectoryIterator($dir, FilesystemIterator::SKIP_DOTS );

$iterator = new Filter1( $iterator );

$iterator = new Filter2( $iterator );

$iterator = new RecursiveIteratorIterator( $iterator, RecursiveIteratorIterator::LEAVES_ONLY, RecursiveIteratorIterator::CATCH_GET_CHILD );

foreach ( $iterator as $item ) {
}
?>
OK
--CLEAN--
<?php
$dir = __DIR__ . '/bug77263';
unlink("$dir/file1");
unlink("$dir/subdir/file2");
rmdir("$dir/subdir");
rmdir($dir);
?>
--EXPECT--
OK
