--TEST--
buildFromIterator with user overrides - wrong return type in getPathname()
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
    #[\ReturnTypeWillChange]
    public function getPathname(): int {
        return 1;
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        return new MyGlobIterator(parent::current()->getPath() . '/*');
    }
}

$workdir = __DIR__.'/006';
mkdir($workdir . '/content', recursive: true);
file_put_contents($workdir . '/content/hello.txt', "Hello world.");

$phar = new \Phar($workdir . "/test.phar");
$phar->startBuffering();
try {
    $phar->buildFromIterator(
        new RecursiveIteratorIterator(
            new MyIterator($workdir . '/content', FilesystemIterator::SKIP_DOTS)
        ),
        $workdir
    );
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
$phar->stopBuffering();

?>
--CLEAN--
<?php
$workdir = __DIR__.'/006';
@unlink($workdir . '/content/hello.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECT--
getPathname() must return a string
