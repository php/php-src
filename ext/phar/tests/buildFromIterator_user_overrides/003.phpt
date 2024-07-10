--TEST--
buildFromIterator with user overrides 003 - errors in getPathname()
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
        throw new Error('exception in getPathname()');
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        echo "[ Found: " . parent::current()->getPathname() . " ]\n";
        return new MyGlobIterator(parent::current()->getPath() . '/*');
    }
}

$workdir = __DIR__.'/003';
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
$workdir = __DIR__.'/003';
@unlink($workdir . '/content/hello.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
[ Found: %shello.txt ]
string(%d) "%shello.txt"
exception in getPathname()
