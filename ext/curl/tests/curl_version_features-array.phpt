--TEST--
Test curl_version() - feature_list functionality
--EXTENSIONS--
curl
--FILE--
<?php
    $info_curl = curl_version();
    print_r(array_map(fn ($v) => get_debug_type($v), $info_curl['feature_list']));

    ob_start();
    phpinfo();
    $phpinfo = ob_get_clean();

    foreach ($info_curl['feature_list'] as $key => $value) {
        if (!is_bool($value)) {
            throw new Exception('Found non-bool value');
        }

        if (!str_contains($phpinfo, $key .' => ' . $value ? 'Yes' : 'No')) {
            throw new Exception($key . ' not found in Curl phpinfo()');
        }
    }

    echo "Complete";
?>
--EXPECTF--
Array
(
    [AsynchDNS] => bool
    [CharConv] => bool
    [Debug] => bool
    [GSS-Negotiate] => bool
    [IDN] => bool
    [IPv6] => bool
    [krb4] => bool
    [Largefile] => bool
    [libz] => bool
    [NTLM] => bool
    [NTLMWB] => bool
    [SPNEGO] => bool
    [SSL] => bool
    [SSPI] => bool
    [TLS-SRP] => bool
    [HTTP2] => bool
    [GSSAPI] => bool
    [KERBEROS5] => bool
    [UNIX_SOCKETS] => bool
    [PSL] => bool
    [HTTPS_PROXY] => bool
    [MULTI_SSL] => bool
    [BROTLI] => bool
%A
)
Complete
