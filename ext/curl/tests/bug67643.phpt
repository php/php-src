--TEST--
Bug #67643 (curl_multi_getcontent returns '' when RETURNTRANSFER isn't set)
--SKIPIF--
<?php
if (!extension_loaded('curl')) print 'skip';
?>
--FILE--
<?php
 	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, 'file://'. dirname(__FILE__) . DIRECTORY_SEPARATOR .'curl_testdata1.txt');
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, false);

	$mh = curl_multi_init();
	curl_multi_add_handle($mh, $ch);

	$running = 0;
	do {
		curl_multi_exec($mh, $running);
	} while($running > 0);

	$results = curl_multi_getcontent($ch);

 	curl_multi_remove_handle($mh, $ch);
	curl_multi_close($mh);

	var_dump($results);
?>
--EXPECT--
CURL1
NULL
