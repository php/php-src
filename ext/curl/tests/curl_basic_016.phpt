--TEST--
Test curl_getinfo() function with basic functionality
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
$curl_version = curl_version();
if ($curl_version['version_number'] > 0x071201) {
    exit("skip: tests works only on older versions of curl");
}
?>
--FILE--
<?php
  $ch   = curl_init();
  $info = curl_getinfo($ch);
  var_dump($info);
?>
===DONE===
--EXPECTF--
array(2%d) {
  ["url"]=>
  string(0) ""
  ["content_type"]=>
  NULL
  ["http_code"]=>
  int(0)
  ["header_size"]=>
  int(0)
  ["request_size"]=>
  int(0)
  ["filetime"]=>
  int(0)
  ["ssl_verify_result"]=>
  int(0)
  ["redirect_count"]=>
  int(0)
  ["total_time"]=>
  float(0)
  ["namelookup_time"]=>
  float(0)
  ["connect_time"]=>
  float(0)
  ["pretransfer_time"]=>
  float(0)
  ["size_upload"]=>
  float(0)
  ["size_download"]=>
  float(0)
  ["speed_download"]=>
  float(0)
  ["speed_upload"]=>
  float(0)
  ["download_content_length"]=>
  float(%f)
  ["upload_content_length"]=>
  float(%f)
  ["starttransfer_time"]=>
  float(0)
  ["redirect_time"]=>
  float(0)
}
===DONE===
