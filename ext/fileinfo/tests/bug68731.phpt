--TEST--
Bug #68731 finfo_buffer doesn't extract the correct mime with some gifs
--SKIPIF--
<?php
if (!class_exists('finfo'))
    die('skip no fileinfo extension');
--FILE--
<?php
    $buffer = file_get_contents(__DIR__ . '/68731.gif');
    $finfo = finfo_open(FILEINFO_MIME_TYPE);
    echo finfo_buffer($finfo, $buffer);
?>
--EXPECT--
image/gif
