--TEST--
stream userfilter test
--FILE--
<?php

class testfilter extends php_user_filter {
  function filter($in, $out, &$consumed, $closing): int {
    while ($bucket = stream_bucket_make_writeable($in)) {
      $bucket->data = strtoupper($bucket->data);
      $consumed += strlen($bucket->data);
      stream_bucket_append($out, $bucket);
    }
    return PSFS_PASS_ON;
  }

  function oncreate(): bool {
    echo "params: {$this->params}\n";
    return true;
  }
}

stream_filter_register('testfilter','testfilter');

$text = "Hello There!";

$fp = tmpfile();
fwrite($fp, $text);

rewind($fp);
stream_filter_append($fp, 'testfilter', STREAM_FILTER_READ, 'testuserfilter');

var_dump(fgets($fp));
fclose($fp);

?>
--EXPECT--
params: testuserfilter
string(12) "HELLO THERE!"
