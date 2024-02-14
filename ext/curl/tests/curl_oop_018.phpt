--TEST--
Test CurlHandle::setOpt() with CurlMultiHandle class with basic functionality
--CREDITS--
Based on curl_basic_018.phpt by:
TestFest 2009 - AFUP - Thomas Rabaix <thomas.rabaix@gmail.com>
--EXTENSIONS--
curl
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();

  // start testing
  echo "*** Testing CurlMultiHandle::exec() : basic functionality ***\n";

  $mh = new CurlMultiHandle;
  $chs = [];
  $url = "{$host}/get.inc?test=get";

  for ($i = 0; $i < 3; ++$i) {
    $chs[$i] = new CurlHandle($url);
    $mh->addHandle($chs[$i]);
  }

  //execute the handles
  $running=null;
  while ($mh->exec($running));

  $curl_content = '';
  for ($i = 0; $i < 3; ++$i) {
    $curl_content .= CurlMultiHandle::getContent($chs[$i]);
    $mh->removeHandle($chs[$i]);
  }
  curl_multi_close($mh);

  var_dump( $curl_content );

?>
--EXPECT--
*** Testing CurlMultiHandle::exec() : basic functionality ***
string(75) "Hello World!
Hello World!Hello World!
Hello World!Hello World!
Hello World!"
