--TEST--
Filter errors: zlib.inflate
--SKIPIF--
<?php require 'filter_errors.inc'; filter_errors_skipif('zlib.inflate'); ?>
--FILE--
<?php
require 'filter_errors.inc';
filter_errors_test('zlib.inflate', gzencode('42'));
?>
--EXPECTF--
test filtering of buffered data

Notice: stream_filter_append(): zlib: data error in %s on line %d

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data

Notice: stream_get_contents(): zlib: data error in %s on line %d
