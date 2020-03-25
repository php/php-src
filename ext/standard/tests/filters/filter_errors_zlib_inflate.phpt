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

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data
