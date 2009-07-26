--TEST--
Test curl_getinfo() function with basic functionality
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php if (!extension_loaded("curl")) exit("skip curl extension not loaded"); ?>
--FILE--
<?php
  $ch   = curl_init();
  $info = curl_getinfo($ch);
  var_dump($info);
?>
===DONE===
--EXPECTF--
array(20) {
  [%u|b%"url"]=>
  string(0) ""
  [u"content_type"]=>
  NULL
  [u"http_code"]=>
  int(0)
  [u"header_size"]=>
  int(0)
  [u"request_size"]=>
  int(0)
  [u"filetime"]=>
  int(0)
  [u"ssl_verify_result"]=>
  int(0)
  [u"redirect_count"]=>
  int(0)
  [u"total_time"]=>
  float(0)
  [u"namelookup_time"]=>
  float(0)
  [u"connect_time"]=>
  float(0)
  [u"pretransfer_time"]=>
  float(0)
  [u"size_upload"]=>
  float(0)
  [u"size_download"]=>
  float(0)
  [u"speed_download"]=>
  float(0)
  [u"speed_upload"]=>
  float(0)
  [u"download_content_length"]=>
  float(0)
  [u"upload_content_length"]=>
  float(%f)
  [u"starttransfer_time"]=>
  float(%f)
  [u"redirect_time"]=>
  float(0)
}
===DONE===
