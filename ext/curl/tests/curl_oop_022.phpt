--TEST--
Test CurlHandle::getInfo() function with CURLINFO_COOKIELIST parameter
--EXTENSIONS--
curl
--FILE--
<?php

$ch = (new CurlHandle)
  ->setOpt(CURLOPT_COOKIELIST, 'Set-Cookie: C1=v1; expires=Thu, 31-Dec-2037 23:59:59 GMT; path=/; domain=.php.net')
  ->setOpt(CURLOPT_COOKIELIST, 'Set-Cookie: C2=v2; expires=Thu, 31-Dec-2037 23:59:59 GMT; path=/; domain=.php.net');

var_dump($ch->getInfo(CURLINFO_COOKIELIST));

--EXPECT--
array(2) {
  [0]=>
  string(38) ".php.net	TRUE	/	FALSE	2145916799	C1	v1"
  [1]=>
  string(38) ".php.net	TRUE	/	FALSE	2145916799	C2	v2"
}
