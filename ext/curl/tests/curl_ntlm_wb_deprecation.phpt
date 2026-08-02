--TEST--
CURLAUTH_NTLM_WB and CURL_VERSION_NTLM_WB constants are deprecated
--EXTENSIONS--
curl
--FILE--
<?php
var_dump(CURLAUTH_NTLM_WB);
var_dump(CURL_VERSION_NTLM_WB);
?>
--EXPECTF--
Deprecated: Constant CURLAUTH_NTLM_WB is deprecated since 8.6, as libcurl 8.8.0 removed NTLM_WB support in %s on line %d
int(%d)

Deprecated: Constant CURL_VERSION_NTLM_WB is deprecated since 8.6, as libcurl 8.8.0 removed NTLM_WB support in %s on line %d
int(%d)
