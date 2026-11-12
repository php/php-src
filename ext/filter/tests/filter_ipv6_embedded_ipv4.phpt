--TEST--
FILTER_VALIDATE_IP: range flags take the IPv4 address embedded in an IPv6 address into account
--EXTENSIONS--
filter
--FILE--
<?php
$addresses = [
    // RFC 6052 - 64:ff9b::/96: classified as the embedded IPv4 address
    '64:ff9b::8.8.8.8', '64:ff9b::808:808',
    '64:ff9b::10.0.0.1', '64:ff9b::a00:1',
    '64:ff9b::127.0.0.1', '64:ff9b::169.254.169.254',
    '64:ff9b::100.64.0.1',
    '64:FF9B::0808:0808', '0064:ff9b:0:0:0:0:808:808', '64:ff9b:0:0:0:0:a9fe:a9fe',
    '64:ff9b::', '64:ff9b::c058:6301',
    // RFC 4291 - IPv4-mapped ::ffff:0:0/96: reserved, private if the IPv4 address is
    '::ffff:8.8.8.8', '::ffff:10.0.0.1', '::FFFF:0A00:0001', '::ffff:c0a8:101', '::ffff:127.0.0.1',
    // RFC 4291 - IPv4-compatible ::/96 (deprecated): same
    '::8.8.8.8', '::808:808', '::10.0.0.1', '::127.0.0.1', '::2',
    // RFC 2765 - IPv4-translated ::ffff:0:0:0/96: same
    '::ffff:0:8.8.8.8', '::ffff:0:10.0.0.1', '::ffff:0:7f00:1',
    // RFC 3056 - 6to4 2002::/16: not global, private or reserved if the IPv4 address is
    '2002:808:808::1', '2002:a00:1::', '2002:c0a8:101::1', '2002:7f00:1::', '2002:a9fe:a9fe::1', '2002:6440:1::',
    // RFC 8215 - 64:ff9b:1::/48: private, wherever the IPv4 address is embedded
    '64:ff9b:1::808:808', '64:ff9b:1:a00:1::', '64:ff9b:1::10.0.0.1',
    // Unchanged
    '::', '::1',
];

foreach ($addresses as $address) {
    $out = [];
    foreach (['global' => FILTER_FLAG_GLOBAL_RANGE, 'no_priv' => FILTER_FLAG_NO_PRIV_RANGE, 'no_res' => FILTER_FLAG_NO_RES_RANGE] as $name => $flag) {
        $out[] = $name . '=' . (filter_var($address, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | $flag) === false ? 'reject' : 'accept');
    }
    echo str_pad($address, 26), implode(' ', $out), "\n";
}
?>
--EXPECT--
64:ff9b::8.8.8.8          global=accept no_priv=accept no_res=accept
64:ff9b::808:808          global=accept no_priv=accept no_res=accept
64:ff9b::10.0.0.1         global=reject no_priv=reject no_res=accept
64:ff9b::a00:1            global=reject no_priv=reject no_res=accept
64:ff9b::127.0.0.1        global=reject no_priv=accept no_res=reject
64:ff9b::169.254.169.254  global=reject no_priv=accept no_res=reject
64:ff9b::100.64.0.1       global=reject no_priv=accept no_res=accept
64:FF9B::0808:0808        global=accept no_priv=accept no_res=accept
0064:ff9b:0:0:0:0:808:808 global=accept no_priv=accept no_res=accept
64:ff9b:0:0:0:0:a9fe:a9fe global=reject no_priv=accept no_res=reject
64:ff9b::                 global=reject no_priv=accept no_res=reject
64:ff9b::c058:6301        global=accept no_priv=accept no_res=accept
::ffff:8.8.8.8            global=reject no_priv=accept no_res=reject
::ffff:10.0.0.1           global=reject no_priv=reject no_res=reject
::FFFF:0A00:0001          global=reject no_priv=reject no_res=reject
::ffff:c0a8:101           global=reject no_priv=reject no_res=reject
::ffff:127.0.0.1          global=reject no_priv=accept no_res=reject
::8.8.8.8                 global=reject no_priv=accept no_res=reject
::808:808                 global=reject no_priv=accept no_res=reject
::10.0.0.1                global=reject no_priv=reject no_res=reject
::127.0.0.1               global=reject no_priv=accept no_res=reject
::2                       global=reject no_priv=accept no_res=reject
::ffff:0:8.8.8.8          global=reject no_priv=accept no_res=reject
::ffff:0:10.0.0.1         global=reject no_priv=reject no_res=reject
::ffff:0:7f00:1           global=reject no_priv=accept no_res=reject
2002:808:808::1           global=reject no_priv=accept no_res=accept
2002:a00:1::              global=reject no_priv=reject no_res=accept
2002:c0a8:101::1          global=reject no_priv=reject no_res=accept
2002:7f00:1::             global=reject no_priv=accept no_res=reject
2002:a9fe:a9fe::1         global=reject no_priv=accept no_res=reject
2002:6440:1::             global=reject no_priv=accept no_res=accept
64:ff9b:1::808:808        global=reject no_priv=reject no_res=accept
64:ff9b:1:a00:1::         global=reject no_priv=reject no_res=accept
64:ff9b:1::10.0.0.1       global=reject no_priv=reject no_res=accept
::                        global=reject no_priv=accept no_res=reject
::1                       global=reject no_priv=accept no_res=reject
