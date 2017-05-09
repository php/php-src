--TEST--
Test function curl_multi_info_read() by calling it with its required argument
--CREDITS--
Ryan Jentzsch ryan.jentzsch@gmail.com 
--FILE--
<?php

//CREATE RESOURCES
$ch1=curl_init();
$ch2=curl_init();

//SET URL AND OTHER OPTIONS
include 'server.inc';
$host = curl_cli_server_start();
curl_setopt($ch1, CURLOPT_URL, "{$host}/get.php?test=getpost&get_param=test");
curl_setopt($ch2, CURLOPT_URL, "file://".dirname(__FILE__). DIRECTORY_SEPARATOR . "curl_testdata2.txt");
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch2, CURLOPT_RETURNTRANSFER, true);
	
//CREATE MULTIPLE CURL HANDLE
$mh=curl_multi_init();

//ADD THE 2 RESOURCES
curl_multi_add_handle($mh,$ch1);
curl_multi_add_handle($mh,$ch2);

//ACTUAL LOGIC
do {
    $status = curl_multi_exec($mh, $active);
    $info = curl_multi_info_read($mh);
    if (false !== $info) {
        var_dump($info);
    }
} while ($status === CURLM_CALL_MULTI_PERFORM || $active);


//CLOSE
curl_multi_remove_handle($mh,$ch1);
curl_multi_remove_handle($mh,$ch2);
curl_multi_close($mh);	
?>
--EXPECTF--
array(3) {
  ["msg"]=>
  int(1)
  ["result"]=>
  int(0)
  ["handle"]=>
  resource(%d) of type (curl)
}
array(3) {
  ["msg"]=>
  int(1)
  ["result"]=>
  int(0)
  ["handle"]=>
  resource(%d) of type (curl)
}
