--TEST--
buildFromIterator with user overrides - negative getMTime()
--EXTENSIONS--
phar
--INI--
phar.readonly=0
phar.require_hash=0
--FILE--
<?php

class MySplFileInfo extends SplFileInfo {
    public function getMTime(): int|false {
        echo "[MTime]\n";
        return -1;
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        return new MySplFileInfo(parent::current()->getPathname());
    }
}

$workdir = __DIR__.'/getMTime_negative';
mkdir($workdir . '/content', recursive: true);
file_put_contents($workdir . '/content/hello.txt', "Hello world.");

try {
    $phar = new \Phar($workdir . "/test.phar");
    $phar->startBuffering();
    $phar->buildFromIterator(
        new RecursiveIteratorIterator(
            new MyIterator($workdir . '/content', FilesystemIterator::SKIP_DOTS)
        ),
        $workdir
    );
    $phar->stopBuffering();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--CLEAN--
<?php
$workdir = __DIR__.'/getMTime_negative';
@unlink($workdir . '/content/hello.txt');
@unlink($workdir . '/test.phar');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
[MTime]
BadMethodCallException: Entry content%chello.txt cannot be created: timestamp is limited to 32-bit
