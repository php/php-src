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
    [alt-svc] => bool
    [AsynchDNS] => bool
    [brotli] => bool
    [GSS-API] => bool
    [HSTS] => bool
    [HTTP2] => bool
    [HTTPS-proxy] => bool
    [IDN] => bool
    [IPv6] => bool
    [Kerberos] => bool
    [Largefile] => bool
    [libz] => bool
    [NTLM] => bool
    [PSL] => bool
    [SPNEGO] => bool
    [SSL] => bool
    [threadsafe] => bool
    [TLS-SRP] => bool
    [UnixSockets] => bool
    [zstd] => bool
%A)
Complete
