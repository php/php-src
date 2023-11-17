--TEST--
Filter errors: zlib.inflate
--SKIPIF--
<?php require 'filter_errors.inc'; filter_errors_skipif('zlib.inflate'); ?>
--FILE--
<?php
require 'filter_errors.inc';
try {
    filter_errors_test('zlib.inflate', gzencode('42'));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
test filtering of buffered data

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
zlib: data error
