--TEST--
GH-11078 (PHP Fatal error triggers pointer being freed was not allocated and malloc: double free for ptr errors)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') die('skip Zend MM disabled');
if (PHP_OS_FAMILY === 'Windows') die('skip Windows does not support generic stream casting');
?>
--FILE--
<?php

const MEM = 32 * 1024 * 1024;
ini_set('memory_limit', MEM);

class CrashingFifo {
    public $context;

    function stream_open($path, $mode, $options, &$opened_path): bool {
        return true;
    }

    function stream_read(int $count): false|string|null {
        return str_repeat('x', MEM);
    }
}

stream_register_wrapper('fifo', CrashingFifo::class);
$readStream = fopen('fifo://1', 'r');
zend_test_cast_fread($readStream);

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %s
