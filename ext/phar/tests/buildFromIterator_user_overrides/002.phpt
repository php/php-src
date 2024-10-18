--TEST--
buildFromIterator with user overrides 002 - errors in getMTime()
--EXTENSIONS--
phar
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip: 64-bit only");
?>
--INI--
phar.readonly=0
phar.require_hash=0
--CREDITS--
Arne Blankerts 
Niels Dossche
--FILE--
<?php

class MySplFileInfo1 extends SplFileInfo {
    public function getMTime(): int|false {
        echo "[MTime]\n";
        return PHP_INT_MAX;
    }
}

class MySplFileInfo2 extends SplFileInfo {
    public function getMTime(): int|false {
        echo "[MTime]\n";
        return false;
    }
}

class MySplFileInfo3 extends SplFileInfo {
    public function getMTime(): int|false {
        echo "[MTime]\n";
        throw new Error('Throwing an exception inside getMTime()');
    }
}

class MyIterator extends RecursiveDirectoryIterator {
    public function current(): SplFileInfo {
        static $counter = 0;
        $counter++;
        echo "[ Found: " . parent::current()->getPathname() . " ]\n";
        if ($counter === 1) {
            return new MySplFileInfo1(parent::current()->getPathname());
        } else if ($counter === 2) {
            return new MySplFileInfo2(parent::current()->getPathname());
        } else if ($counter === 3) {
            return new MySplFileInfo3(parent::current()->getPathname());
        }
    }
}

$workdir = __DIR__.'/002';
mkdir($workdir . '/content', recursive: true);
file_put_contents($workdir . '/content/hello.txt', "Hello world.");

for ($i = 0; $i < 3; $i++) {
    echo "--- Iteration $i ---\n";
    try {
        $phar = new \Phar($workdir . "/test$i.phar");
        $phar->startBuffering();
        $phar->buildFromIterator(
            new RecursiveIteratorIterator(
                new MyIterator($workdir . '/content', FilesystemIterator::SKIP_DOTS)
            ),
            $workdir
        );
        $phar->stopBuffering();
    } catch (Throwable $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--CLEAN--
<?php
$workdir = __DIR__.'/002';
@unlink($workdir . '/content/hello.txt');
@rmdir($workdir . '/content');
@rmdir($workdir);
?>
--EXPECTF--
--- Iteration 0 ---
[ Found: %shello.txt ]
[MTime]
Entry content%chello.txt cannot be created: timestamp is limited to 32-bit
--- Iteration 1 ---
[ Found: %shello.txt ]
[MTime]
Entry content%chello.txt cannot be created: getMTime() failed
--- Iteration 2 ---
[ Found: %shello.txt ]
[MTime]
Throwing an exception inside getMTime()
