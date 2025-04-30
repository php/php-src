--TEST--
inet_ntop() & inet_pton() IPv6 tests
--SKIPIF--
<?php

$packed = str_repeat(chr(0), 15) . chr(1);
if (@inet_ntop($packed) === false) {
    die("skip no IPv6 support");
}
if (stristr(PHP_OS, "darwin") !== false) die("skip MacOS has broken inet_*() funcs");
?>
--FILE--
<?php

$a = array(
    '::1',
    '::2',
    '::35',
    '::255',
    '::1024',
    '',
    '2001:0db8:85a3:08d3:1319:8a2e:0370:7344',
    '2001:0db8:1234:0000:0000:0000:0000:0000',
    '2001:0db8:1234:FFFF:FFFF:FFFF:FFFF:FFFF',
);

foreach ($a as $address) {
    $packed = inet_pton($address);
    var_dump(inet_ntop($packed));
}

echo "Done\n";
?>
--EXPECT--
string(3) "::1"
string(3) "::2"
string(4) "::35"
string(5) "::255"
string(6) "::1024"
bool(false)
string(36) "2001:db8:85a3:8d3:1319:8a2e:370:7344"
string(15) "2001:db8:1234::"
string(38) "2001:db8:1234:ffff:ffff:ffff:ffff:ffff"
Done
