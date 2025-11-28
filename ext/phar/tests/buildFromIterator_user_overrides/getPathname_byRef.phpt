--TEST--
buildFromIterator with user overrides - getPathname() by ref
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--CREDITS--
Arne Blankerts 
Niels Dossche
--FILE--
<?php

class MyGlobIterator extends GlobIterator {
    public function &getPathname(): string {
        static $data = parent::getPathname();
        return $data;
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        return new MyGlobIterator(parent::current()->getPath() . '/*');
    }
}

$workdir = __DIR__.'/005';
mkdir($workdir . '/content', recursive: true);
file_put_contents($workdir . '/content/hello.txt', "Hello world 1.");

$phar = new \Phar($workdir . "/test.phar");
$phar->startBuffering();
$phar->buildFromIterator(
    new RecursiveIteratorIterator(
        new MyIterator($workdir . '/content', FilesystemIterator::SKIP_DOTS)
    ),
    $workdir
);
$phar->stopBuffering();

$result = new \Phar($workdir . '/test.phar', 0, 'test.phar');
var_dump(isset($result['content/hello.txt']));

?>
--CLEAN--
<?php
$workdir = __DIR__.'/005';
@unlink($workdir . '/test.phar');
@unlink($workdir . '/content/hello.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
bool(true)
