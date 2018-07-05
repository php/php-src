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
--EXPECT--
test bold italic test
test <b>bold</b> italic test
test <b>bold</b> italic test
