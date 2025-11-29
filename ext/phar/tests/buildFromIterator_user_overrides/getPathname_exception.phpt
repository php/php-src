--TEST--
buildFromIterator with user overrides - exception in getPathname()
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--CREDITS--
Arne Blankerts
N. Dossche
--FILE--
<?php

class MyGlobIterator extends GlobIterator {
    public function getPathname(): string {
        echo "[getPathname]\n";
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

$workdir = __DIR__.'/getPathname_exception';
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
    if ($previous = $e->getPrevious()) {
        echo "Previous: ", $previous->getMessage(), "\n";
    }
}
$phar->stopBuffering();

?>
--CLEAN--
<?php
$workdir = __DIR__.'/getPathname_exception';
@unlink($workdir . '/content/hello.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
[ Found: %shello.txt ]
[getPathname]
string(%d) "%shello.txt"
getPathname() must return a string
Previous: exception in getPathname()
