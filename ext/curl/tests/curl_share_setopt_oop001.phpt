--TEST--
CurlShareHandle::setOpt() basic test
--EXTENSIONS--
curl
--FILE--
<?php

$sh = new CurlShareHandle;
$sh->setOpt(CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE)
   ->setOpt(CURLSHOPT_UNSHARE, CURL_LOCK_DATA_COOKIE);

try {
	echo "Setting invalid option\n";
	$sh->setOpt(-1, 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Setting invalid option
CurlShareHandle::setOpt(): Argument #1 ($option) is not a valid cURL share option
