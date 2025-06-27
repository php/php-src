--TEST--
Phar: Stream double free
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php

declare(strict_types=1);

require __DIR__ . '/gh18953/autoload.inc';

// cleaning
@unlink("gh18953.phar");
@unlink("gh18953.phar.gz");

// create a phar
$phar = new Phar("gh18953.phar");
$phar->startBuffering();
// add any dir
$phar->addEmptyDir("dir");
$phar->stopBuffering();
// compress
$phar->compress(Phar::GZ);

// this increases the chance of reproducing the problem
// even with this, it's not always reproducible
$obj1 = new NS1\Class1();
$obj2 = new NS1\Class1();
$obj2 = new NS1\Class1();
$obj2 = new NS1\Class1();
$obj2 = new NS1\Class1();

echo "Done" . PHP_EOL;
?>
--CLEAN--
<?php
@unlink("gh18953.phar");
@unlink("gh18953.phar.gz");
?>
--EXPECT--
Done
