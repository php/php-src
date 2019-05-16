--TEST--
Request #72189 (Add missing CURL_VERSION_* constants)
--SKIPIF--
<?php

include 'skipif.inc';

$version = curl_version();

if ($version['version_number'] < 0x071306) {
    exit('skip: test works only with curl >= 7.19.6');
}

?>
--FILE--
<?php

$version = curl_version();

$bitfields = [
    CURL_VERSION_ASYNCHDNS,
    CURL_VERSION_CONV,
    CURL_VERSION_CURLDEBUG,
    CURL_VERSION_DEBUG,
    CURL_VERSION_GSSNEGOTIATE,
    CURL_VERSION_IDN,
    CURL_VERSION_IPV6,
    CURL_VERSION_KERBEROS4,
    CURL_VERSION_LARGEFILE,
    CURL_VERSION_LIBZ,
    CURL_VERSION_NTLM,
    CURL_VERSION_SPNEGO,
    CURL_VERSION_SSL,
    CURL_VERSION_SSPI,
];

$matchesCount = 0;

foreach ($bitfields as $feature) {
    if ($version['features'] & $feature) {
        ++$matchesCount;
    }
}

var_dump($matchesCount > 0);

?>
--EXPECT--
bool(true)
