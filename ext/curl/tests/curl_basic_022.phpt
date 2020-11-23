--TEST--
Test curl_getinfo() function with CURLINFO_COOKIELIST parameter
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php

$ch = curl_init();
curl_setopt($ch, CURLOPT_COOKIELIST, 'Set-Cookie: C1=v1; expires=Thu, 31-Dec-2037 23:59:59 GMT; path=/; domain=.php.net');
curl_setopt($ch, CURLOPT_COOKIELIST, 'Set-Cookie: C2=v2; expires=Thu, 31-Dec-2037 23:59:59 GMT; path=/; domain=.php.net');
var_dump(curl_getinfo($ch, CURLINFO_COOKIELIST));

?>
--EXPECT--
array(2) {
  [0]=>
  string(38) ".php.net	TRUE	/	FALSE	2145916799	C1	v1"
  [1]=>
  string(38) ".php.net	TRUE	/	FALSE	2145916799	C2	v2"
}
