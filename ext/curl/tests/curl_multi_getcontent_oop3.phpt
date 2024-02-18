--TEST--
CurlMultiHandle::getContent() basic test with different sources (local file/http)
--CREDITS--
Based on curl_multi_getcontent_basic3.phpt by:
Rein Velt (rein@velt.org)
#TestFest Utrecht 20090509
--EXTENSIONS--
curl
--FILE--
<?php
try {
    //CURL_MULTI_GETCONTENT TEST


    //SET URL AND OTHER OPTIONS
    include 'server.inc';
    $host = curl_cli_server_start();

    //CREATE RESOURCES
	$ch1 = (new CurlHandle)
		->setOpt(CURLOPT_URL, "{$host}/get.inc?test=getpost&get_param=Hello%20World")
		->setOpt(CURLOPT_RETURNTRANSFER, true);

	$ch2 = (new CurlHandle)
		->setOpt(CURLOPT_URL, "file://" . __DIR__ . DIRECTORY_SEPARATOR . "curl_testdata2.txt")
		->setOpt(CURLOPT_RETURNTRANSFER, true);

    //CREATE MULTIPLE CURL HANDLE
    $mh = (new CurlMultiHandle)
		->addHandle($ch1)
		->addHandle($ch2);

    //EXECUTE
    $running=0;
    while ($mh->exec($running));

	$results1 = CurlMultiHandle::getContent($ch1);
	$results2 = CurlMultiHandle::getContent($ch2);

    //CLOSE
	$mh->removeHandle($ch1)->removeHandle($ch2);

    echo $results1;
    echo $results2;
} catch (\Throwable $e) {
  var_dump($e);
}
?>
--EXPECT--
array(2) {
  ["test"]=>
  string(7) "getpost"
  ["get_param"]=>
  string(11) "Hello World"
}
array(0) {
}
CURL2
