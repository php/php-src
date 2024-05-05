--TEST--
buildFromIterator with user overrides 004 - getPathname()
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
    public function getPathname(): string {
        var_dump(parent::getPathname());
        // For testing: always return hello2 such that it will be the only file
        return str_replace("hello1.txt", "hello2.txt", parent::getPathname());
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        echo "[ Found: " . parent::current()->getPathname() . " ]\n";
        return new MyGlobIterator(parent::current()->getPath() . '/*');
    }
}

$workdir = __DIR__.'/004';
mkdir($workdir . '/content', recursive: true);
file_put_contents($workdir . '/content/hello1.txt', "Hello world 1.");
file_put_contents($workdir . '/content/hello2.txt', "Hello world 2.");

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
var_dump(isset($result['content/hello1.txt']));
var_dump(isset($result['content/hello2.txt']));

?>
--CLEAN--
<?php
$workdir = __DIR__.'/004';
@unlink($workdir . '/test.phar');
@unlink($workdir . '/content/hello1.txt');
@unlink($workdir . '/content/hello2.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
[ Found: %shello2.txt ]
string(%d) "%shello1.txt"
[ Found: %shello1.txt ]
string(%d) "%shello1.txt"
bool(false)
bool(true)
