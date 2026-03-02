--TEST--
GH-10370: File corruption in _php_stream_copy_to_stream_ex when using copy_file_range - partial copy using stream_copy_to_stream
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (PHP_OS != 'Linux') {
    die('skip For Linux only');
}
?>
--INI--
zend_test.limit_copy_file_range=3584
--FILE--
<?php
/* Note: the value 3584 is chosen so that the mmap in _php_stream_copy_to_stream_ex() will mmap
 *       at an offset of a multiple of 4096, which is the standard page size in most Linux systems. */
mkdir(__DIR__ . DIRECTORY_SEPARATOR . 'gh10370_003');

$input = fopen(__DIR__ . DIRECTORY_SEPARATOR . 'gh10370.tar', 'r');
$output = fopen(__DIR__ . DIRECTORY_SEPARATOR . 'gh10370_003' . DIRECTORY_SEPARATOR . 'testfile', 'w');

var_dump(stream_copy_to_stream($input, $output, 10240, 0x200));

fclose($input);
fclose($output);

var_dump(sha1_file(__DIR__ . DIRECTORY_SEPARATOR . 'gh10370_003' . DIRECTORY_SEPARATOR . 'testfile'));
?>
--EXPECT--
int(10240)
string(40) "a723ae4ec7eababff73ca961a771b794be6388d2"
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . 'gh10370_003' . DIRECTORY_SEPARATOR . 'testfile');
@rmdir(__DIR__ . DIRECTORY_SEPARATOR . 'gh10370_003');
?>
