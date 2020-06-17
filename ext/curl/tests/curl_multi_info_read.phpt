--TEST--
array curl_multi_info_read ( resource $mh [, int &$msgs_in_queue = NULL ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--SKIPIF--
<?php
if (!extension_loaded('curl')) { print("skip"); }
?>
--FILE--
<?php
$urls = array(
    "file://".__DIR__."/curl_testdata1.txt",
    "file://".__DIR__."/curl_testdata2.txt",
);

$mh = curl_multi_init();
foreach ($urls as $i => $url) {
    $conn[$i] = curl_init($url);
    curl_setopt($conn[$i], CURLOPT_RETURNTRANSFER, 1);
    curl_multi_add_handle($mh, $conn[$i]);
}

do {
    $status = curl_multi_exec($mh, $active);
} while ($status === CURLM_CALL_MULTI_PERFORM || $active);

while ($info = curl_multi_info_read($mh)) {
    var_dump($info);
}

foreach ($urls as $i => $url) {
    curl_close($conn[$i]);
}
?>
--EXPECTF--
array(3) {
  ["msg"]=>
  int(%d)
  ["result"]=>
  int(%d)
  ["handle"]=>
  object(CurlHandle)#%d (0) {
  }
}
array(3) {
  ["msg"]=>
  int(%d)
  ["result"]=>
  int(%d)
  ["handle"]=>
  object(CurlHandle)#%d (0) {
  }
}
