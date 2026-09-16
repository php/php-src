--TEST--
curl error constants
--EXTENSIONS--
curl
--FILE--
<?php

$expected = [
    'CURLE_OK' => 0,
    'CURLE_UNSUPPORTED_PROTOCOL' => 1,
    'CURLE_FAILED_INIT' => 2,
    'CURLE_URL_MALFORMAT' => 3,
    'CURLE_URL_MALFORMAT_USER' => 4,
    'CURLE_COULDNT_RESOLVE_PROXY' => 5,
    'CURLE_COULDNT_RESOLVE_HOST' => 6,
    'CURLE_COULDNT_CONNECT' => 7,
    'CURLE_WEIRD_SERVER_REPLY' => 8,
    'CURLE_FTP_WEIRD_SERVER_REPLY' => 8,
    'CURLE_FTP_ACCESS_DENIED' => 9,
    'CURLE_FTP_USER_PASSWORD_INCORRECT' => 10,
    'CURLE_FTP_WEIRD_PASS_REPLY' => 11,
    'CURLE_FTP_WEIRD_USER_REPLY' => 12,
    'CURLE_FTP_WEIRD_PASV_REPLY' => 13,
    'CURLE_FTP_WEIRD_227_FORMAT' => 14,
    'CURLE_FTP_CANT_GET_HOST' => 15,
    'CURLE_FTP_CANT_RECONNECT' => 16,
    'CURLE_FTP_COULDNT_SET_BINARY' => 17,
    'CURLE_PARTIAL_FILE' => 18,
    'CURLE_FTP_PARTIAL_FILE' => 18,
    'CURLE_FTP_COULDNT_RETR_FILE' => 19,
    'CURLE_FTP_WRITE_ERROR' => 20,
    'CURLE_FTP_QUOTE_ERROR' => 21,
    'CURLE_HTTP_RETURNED_ERROR' => 22,
    'CURLE_HTTP_NOT_FOUND' => 22,
    'CURLE_WRITE_ERROR' => 23,
    'CURLE_MALFORMAT_USER' => 24,
    'CURLE_FTP_COULDNT_STOR_FILE' => 25,
    'CURLE_READ_ERROR' => 26,
    'CURLE_OUT_OF_MEMORY' => 27,
    'CURLE_OPERATION_TIMEDOUT' => 28,
    'CURLE_OPERATION_TIMEOUTED' => 28,
    'CURLE_FTP_COULDNT_SET_ASCII' => 29,
    'CURLE_FTP_PORT_FAILED' => 30,
    'CURLE_FTP_COULDNT_USE_REST' => 31,
    'CURLE_FTP_COULDNT_GET_SIZE' => 32,
    'CURLE_HTTP_RANGE_ERROR' => 33,
    'CURLE_HTTP_POST_ERROR' => 34,
    'CURLE_SSL_CONNECT_ERROR' => 35,
    'CURLE_BAD_DOWNLOAD_RESUME' => 36,
    'CURLE_FTP_BAD_DOWNLOAD_RESUME' => 36,
    'CURLE_FILE_COULDNT_READ_FILE' => 37,
    'CURLE_LDAP_CANNOT_BIND' => 38,
    'CURLE_LDAP_SEARCH_FAILED' => 39,
    'CURLE_LIBRARY_NOT_FOUND' => 40,
    'CURLE_FUNCTION_NOT_FOUND' => 41,
    'CURLE_ABORTED_BY_CALLBACK' => 42,
    'CURLE_BAD_FUNCTION_ARGUMENT' => 43,
    'CURLE_BAD_CALLING_ORDER' => 44,
    'CURLE_HTTP_PORT_FAILED' => 45,
    'CURLE_BAD_PASSWORD_ENTERED' => 46,
    'CURLE_TOO_MANY_REDIRECTS' => 47,
    'CURLE_UNKNOWN_TELNET_OPTION' => 48,
    'CURLE_TELNET_OPTION_SYNTAX' => 49,
    'CURLE_OBSOLETE' => 50,
    'CURLE_GOT_NOTHING' => 52,
    'CURLE_SSL_ENGINE_NOTFOUND' => 53,
    'CURLE_SSL_ENGINE_SETFAILED' => 54,
    'CURLE_SEND_ERROR' => 55,
    'CURLE_RECV_ERROR' => 56,
    'CURLE_SHARE_IN_USE' => 57,
    'CURLE_SSL_CERTPROBLEM' => 58,
    'CURLE_SSL_CIPHER' => 59,
    'CURLE_SSL_CACERT' => 60,
    'CURLE_SSL_PEER_CERTIFICATE' => 60,
    'CURLE_BAD_CONTENT_ENCODING' => 61,
    'CURLE_LDAP_INVALID_URL' => 62,
    'CURLE_FILESIZE_EXCEEDED' => 63,
    'CURLE_FTP_SSL_FAILED' => 64,
    'CURLE_SSL_CACERT_BADFILE' => 77,
    'CURLE_SSH' => 79,
    'CURLE_SSL_PINNEDPUBKEYNOTMATCH' => 90,
];

$optional = [
    'CURLE_PROXY' => 97,
];

$defined = array_filter(
    get_defined_constants(),
    static fn (string $name): bool => str_starts_with($name, 'CURLE_'),
    ARRAY_FILTER_USE_KEY,
);

foreach ($expected as $name => $value) {
    if (!array_key_exists($name, $defined)) {
        echo "missing: $name\n";
    } elseif ($defined[$name] !== $value) {
        echo "wrong value: $name is {$defined[$name]}, expected $value\n";
    }
}

foreach ($optional as $name => $value) {
    if (array_key_exists($name, $defined) && $defined[$name] !== $value) {
        echo "wrong value: $name is {$defined[$name]}, expected $value\n";
    }
}

foreach (array_diff_key($defined, $expected, $optional) as $name => $value) {
    echo "not covered by this test: $name = $value\n";
}

echo "done\n";

?>
--EXPECT--
done
