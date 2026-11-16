--TEST--
FILTER_VALIDATE_IP: IPv6 with dotted IPv4 suffix is range-checked like its hexadecimal form
--EXTENSIONS--
filter
--FILE--
<?php
$pairs = [
    'fc00::1.2.3.4'              => 'fc00::102:304',
    'fd00::abcd:10.0.0.1'        => 'fd00::abcd:a00:1',
    '64:ff9b::10.0.0.1'          => '64:ff9b::a00:1',
    'fe80::127.0.0.1'            => 'fe80::7f00:1',
    '2001:db8::1.2.3.4'          => '2001:db8::102:304',
    '2606:4700::8.8.8.8'         => '2606:4700::808:808',
    'fc00:1:2:3:4::1.2.3.4'      => 'fc00:1:2:3:4:0:102:304',
    '1:2:3:4:5:6:1.2.3.4'        => '1:2:3:4:5:6:102:304',
    '1::1.2.3.4'                 => '1::102:304',
    '::ffff:1.2.3.4'             => '::ffff:102:304',
    '::1:1.2.3.4'                => '::1:102:304',
    '::1.2.3.4'                  => '::102:304',
    '::127.0.0.1'                => '::7f00:1',
    '::0.0.0.0'                  => '::',
    '::0.0.0.1'                  => '::1',
    '1:2:3:4:5::6:1.2.3.4'       => '1:2:3:4:5::6:102:304',
    '1:2:3:4:5:6:7:1.2.3.4'      => '1:2:3:4:5:6:7:102:304',
    ':1.2.3.4'                   => ':102:304',
];

$flags = [
    'none'    => 0,
    'global'  => FILTER_FLAG_GLOBAL_RANGE,
    'no_priv' => FILTER_FLAG_NO_PRIV_RANGE,
    'no_res'  => FILTER_FLAG_NO_RES_RANGE,
];

function check(string $ip, int $flag): string {
    return filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6 | $flag) === false ? 'reject' : 'accept';
}

foreach ($pairs as $dotted => $hex) {
    $out = [];
    foreach ($flags as $name => $flag) {
        $d = check($dotted, $flag);
        $h = check($hex, $flag);
        $out[] = "$name=$d" . ($d === $h ? '' : " (hex: $h)");
    }
    echo str_pad($dotted, 24), implode(' ', $out), "\n";
}
?>
--EXPECT--
fc00::1.2.3.4           none=accept global=reject no_priv=reject no_res=accept
fd00::abcd:10.0.0.1     none=accept global=reject no_priv=reject no_res=accept
64:ff9b::10.0.0.1       none=accept global=reject no_priv=reject no_res=accept
fe80::127.0.0.1         none=accept global=reject no_priv=accept no_res=reject
2001:db8::1.2.3.4       none=accept global=reject no_priv=accept no_res=accept
2606:4700::8.8.8.8      none=accept global=accept no_priv=accept no_res=accept
fc00:1:2:3:4::1.2.3.4   none=accept global=reject no_priv=reject no_res=accept
1:2:3:4:5:6:1.2.3.4     none=accept global=accept no_priv=accept no_res=accept
1::1.2.3.4              none=accept global=accept no_priv=accept no_res=accept
::ffff:1.2.3.4          none=accept global=reject no_priv=accept no_res=reject
::1:1.2.3.4             none=accept global=accept no_priv=accept no_res=accept
::1.2.3.4               none=accept global=reject no_priv=accept no_res=reject
::127.0.0.1             none=accept global=reject no_priv=accept no_res=reject
::0.0.0.0               none=accept global=reject no_priv=accept no_res=reject
::0.0.0.1               none=accept global=reject no_priv=accept no_res=reject
1:2:3:4:5::6:1.2.3.4    none=reject global=reject no_priv=reject no_res=reject
1:2:3:4:5:6:7:1.2.3.4   none=reject global=reject no_priv=reject no_res=reject
:1.2.3.4                none=reject global=reject no_priv=reject no_res=reject
