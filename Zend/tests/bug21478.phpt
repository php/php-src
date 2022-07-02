--TEST--
Bug #21478 (Zend/zend_alloc.c :: shutdown_memory_manager produces segfault)
--FILE--
<?php
class debugfilter extends php_user_filter {
  function filter($in, $out, &$consumed, $closing): int {
    while ($bucket = stream_bucket_make_writeable($in)) {
      $bucket->data = strtoupper($bucket->data);
      stream_bucket_append($out, $bucket);
      $consumed += strlen($bucket->data);
    }
    return PSFS_PASS_ON;
  }
}

stream_filter_register("myfilter","debugfilter");

$fp = fopen(__DIR__."/test.txt","w");
stream_filter_append($fp, "myfilter");
stream_filter_append($fp, "myfilter");
stream_filter_append($fp, "myfilter");
fwrite($fp, "This is a test.\n");
print "Done.\n";
fclose($fp);
// Uncommenting the following 'print' line causes the segfault to stop occurring
// print "2\n";
readfile(__DIR__."/test.txt");
unlink(__DIR__."/test.txt");
?>
--EXPECT--
Done.
THIS IS A TEST.
