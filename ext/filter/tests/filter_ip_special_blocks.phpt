--TEST--
FILTER_VALIDATE_IP: site-local, multicast, documentation (3fff::/20) and SRv6 blocks
--EXTENSIONS--
filter
--FILE--
<?php
$addresses = [
    // RFC 3879 - Site-Local Unicast fec0::/10 (deprecated): private
    'fec0::1', 'feff:ffff::1',
    // RFC 4291 - Multicast ff00::/8: reserved
    'ff02::1', 'ff05::1:3', 'ffff::1',
    // RFC 9637 - Documentation 3fff::/20: not global
    '3fff::1', '3fff:fff::1', '3fff:1000::1',
    // RFC 9602 - SRv6 SIDs 5f00::/16: not global
    '5f00::1', '5f01::1',
    // RFC 5771 - IPv4 Multicast 224.0.0.0/4: reserved
    '223.255.255.255', '224.0.0.0', '239.255.255.255',
    // Neighbours, unchanged
    'fe80::1', 'fdff::1',
];

foreach ($addresses as $address) {
    $out = [];
    foreach (['global' => FILTER_FLAG_GLOBAL_RANGE, 'no_priv' => FILTER_FLAG_NO_PRIV_RANGE, 'no_res' => FILTER_FLAG_NO_RES_RANGE] as $name => $flag) {
        $out[] = $name . '=' . (filter_var($address, FILTER_VALIDATE_IP, $flag) === false ? 'reject' : 'accept');
    }
    echo str_pad($address, 18), implode(' ', $out), "\n";
}
?>
--EXPECT--
fec0::1           global=reject no_priv=reject no_res=accept
feff:ffff::1      global=reject no_priv=reject no_res=accept
ff02::1           global=reject no_priv=accept no_res=reject
ff05::1:3         global=reject no_priv=accept no_res=reject
ffff::1           global=reject no_priv=accept no_res=reject
3fff::1           global=reject no_priv=accept no_res=accept
3fff:fff::1       global=reject no_priv=accept no_res=accept
3fff:1000::1      global=accept no_priv=accept no_res=accept
5f00::1           global=reject no_priv=accept no_res=accept
5f01::1           global=accept no_priv=accept no_res=accept
223.255.255.255   global=accept no_priv=accept no_res=accept
224.0.0.0         global=reject no_priv=accept no_res=reject
239.255.255.255   global=reject no_priv=accept no_res=reject
fe80::1           global=reject no_priv=accept no_res=reject
fdff::1           global=reject no_priv=reject no_res=accept
