--TEST--
filter_var() and IPv6
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$ipv6_test = array(
    "::127.0.0.1"					=> true,
    "FF01::101:127.0.1" 			=> false,
    "FF01:0:0:0:101:127.0.1.1"		=> false,
    "FF01:0:0:0:237:101:127.0.1.1"	=> true,
    "FF01::101"						=> true,
    "A1080::8:800:200C:417A"		=> false,
    "1080::8:Z00:200C:417A"			=> false,
    "FF01::101::1"					=> false,
    "1080::8:800:200C:417A"			=> true,
    "1080:0:0:0:8:800:200C:417A"	=> true,
    "2001:ec8:1:1:1:1:1:1"			=> true,
    "ffff::FFFF:129.144.52.38"		=> true,
    "::ffff:1.2.3.4"				=> true,
    "0:0:0:0:0:FFFF:129.144.52.38"	=> true,
    "0:0:0:0:0:0:13.1.68.3"			=> true,
    "0:0:0:0:0:0:0:13.1.68.3"		=> false,
    "::13.1.68.3"					=> true,
    "::FFFF:129.144.52.38"			=> true,

    "1:2:3:4:5:6::129.144.52.38"	=> false,
    "::1:2:3:4:5:6:129.144.52.38"	=> false,
    "1:2:3::4:5:6:129.144.52.38"	=> false,

    "1:2:3:4::5:6:7:8"				=> false,
    "::1:2:3:4:5:6:7"				=> true,
    "::1:2:3:4:5:6:7:8"				=> false,
    "1:2:3:4:5:6:7::"				=> true,
    "1:2:3:4:5:6:7:8::"				=> false,
    "1:2:3:4:5:6:7::8"				=> false,

    "1:2:3:4:5:6:7:8g"				=> false,
    "1:2:3:4:5:6:7:g"				=> false,
    "1:2:3:4:5g:6:7:8"				=> false,

    'a:b:c:d:e::1.2.3.4'			=> true,
    '::0:a:b:c:d:e:f'				=> true,
    '0:a:b:c:d:e:f::'				=> true,
    ':::1.2.3.4'					=> false,
    '8:::1.2.3.4'					=> false,
    '::01.02.03.04'					=> false,
    '::1.00.3.4'					=> false,
    '0:0:0:255.255.255.255'			=> false,
    '0:0:0::255.255.255.255'		=> true,
);
foreach ($ipv6_test as $ip => $exp) {
    $out = filter_var($ip, FILTER_VALIDATE_IP, FILTER_FLAG_IPV6);
    $out = $out !== false;
    if ($exp != $out) {
        echo "$ip failed (expected ", $exp?"true":"false", ", got ",
            $out?"true":"false", ")\n";
    }
}

echo "Ok\n";
?>
--EXPECT--
Ok
