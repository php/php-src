--TEST--
buildFromIterator with user overrides 001 - getMTime()
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

class MySplFileInfo extends SplFileInfo {
    public function getPathname(): string {
        echo "[Pathname]\n";
        return parent::getPathname();
    }

    public function getMTime(): int|false {
        echo "[MTime]\n";
        return 123;
    }

    public function getCTime(): int {
        // This should not get called
        echo "[CTime]\n";
        return 0;
    }

    public function getATime(): int {
        // This should not get called
        echo "[ATime]\n";
        return 0;
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        echo "[ Found: " . parent::current()->getPathname() . " ]\n";
        return new MySplFileInfo(parent::current()->getPathname());
    }
}

$workdir = __DIR__.'/001';
mkdir($workdir . '/content', recursive: true);
file_put_contents($workdir . '/content/hello.txt', "Hello world.");

$phar = new \Phar($workdir . '/test.phar');
$phar->startBuffering();
$phar->buildFromIterator(
    new RecursiveIteratorIterator(
        new MyIterator($workdir . '/content', FilesystemIterator::SKIP_DOTS)
    ),
    $workdir
);
$phar->stopBuffering();


$result = new \Phar($workdir . '/test.phar', 0, 'test.phar');
var_dump($result['content/hello.txt']);
var_dump($result['content/hello.txt']->getATime());
var_dump($result['content/hello.txt']->getMTime());
var_dump($result['content/hello.txt']->getCTime());

?>
--CLEAN--
<?php
$workdir = __DIR__.'/001';
@unlink($workdir . '/test.phar');
@unlink($workdir . '/content/hello.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
[ Found: %shello.txt ]
[Pathname]
[MTime]
object(PharFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "phar://%shello.txt"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "hello.txt"
}
int(123)
int(123)
int(123)
