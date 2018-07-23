--TEST--
string.strip_tags filter
--FILE--
<?php
$fp = fopen('php://output', 'w');
stream_filter_append($fp, 'string.strip_tags');
fwrite($fp, "test <b>bold</b> <i>italic</i> test\n");
fclose($fp);

$fp = fopen('php://output', 'w');
stream_filter_append($fp, 'string.strip_tags', STREAM_FILTER_WRITE, "<b>");
fwrite($fp, "test <b>bold</b> <i>italic</i> test\n");
fclose($fp);

$fp = fopen('php://output', 'w');
stream_filter_append($fp, 'string.strip_tags', STREAM_FILTER_WRITE, ["b"]);
fwrite($fp, "test <b>bold</b> <i>italic</i> test\n");
fclose($fp);

?>
--EXPECTF--
Deprecated: stream_filter_append(): The string.strip_tags filter is deprecated in %s on line %d
test bold italic test

Deprecated: stream_filter_append(): The string.strip_tags filter is deprecated in %s on line %d
test <b>bold</b> italic test

Deprecated: stream_filter_append(): The string.strip_tags filter is deprecated in %s on line %d
test <b>bold</b> italic test
