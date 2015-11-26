--TEST--
Bug #21478 (Zend/zend_alloc.c :: shutdown_memory_manager produces segfault) 
--SKIPIF--
<?php 
	if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 is needed'); 
	if (!function_exists('stream_filter_register')) die('skip stream_filter_register() not available');
?>
--FILE--
<?php
class debugfilter extends php_user_filter {
  function filter($in, $out, &$consumed, $closing) {
    while ($bucket = stream_bucket_make_writeable($in)) {
      $bucket->data = strtoupper($bucket->data);
      stream_bucket_append($out, $bucket);
      $consumed += strlen($bucket->data);
    }
    return PSFS_PASS_ON;
  }
}

stream_filter_register("myfilter","debugfilter");

$fp = fopen(dirname(__FILE__)."/test.txt","w");
stream_filter_append($fp, "myfilter");
stream_filter_append($fp, "myfilter");
stream_filter_append($fp, "myfilter");
fwrite($fp, "This is a test.\n");
print "Done.\n";
fclose($fp);
// Uncommenting the following 'print' line causes the segfault to stop occurring
// print "2\n";  
readfile(dirname(__FILE__)."/test.txt");
unlink(dirname(__FILE__)."/test.txt");
?>
--EXPECT--
Done.
THIS IS A TEST.
