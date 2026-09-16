--TEST--
GH-17650 (realloc with size 0 in user_filters.c)
--FILE--
<?php
class testfilter extends php_user_filter {
  function filter($in, $out, &$consumed, $closing): int {
    while ($bucket = stream_bucket_make_writeable($in)) {
      $bucket->data = '';
      $consumed += strlen($bucket->data);
      stream_bucket_append($out, $bucket);
    }
    return PSFS_PASS_ON;
  }
}

stream_filter_register('testfilter','testfilter');

$text = "Hello There!";

$fp = fopen('php://memory', 'w+');
fwrite($fp, $text);

rewind($fp);
stream_filter_append($fp, 'testfilter', STREAM_FILTER_READ, 'testuserfilter');

while ($x = fgets($fp)) {
  var_dump($x);
}

fclose($fp);

echo "Done\n";
?>
--EXPECT--
Done
