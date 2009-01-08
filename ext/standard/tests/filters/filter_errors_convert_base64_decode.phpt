--TEST--
Filter errors: convert.base64-decode
--SKIPIF--
<?php require 'filter_errors.inc'; filter_errors_skipif('convert.base64-decode'); ?>
--FILE--
<?php
require 'filter_errors.inc';
filter_errors_test('convert.base64-decode', '===');
?>
--EXPECTF--
test filtering of buffered data

Warning: stream_filter_append(): stream filter (convert.base64-decode): invalid byte sequence in %s

Warning: stream_filter_append(): Filter failed to process pre-buffered data in %s
test filtering of non buffered data

Warning: stream_get_contents(): stream filter (convert.base64-decode): invalid byte sequence in %s
