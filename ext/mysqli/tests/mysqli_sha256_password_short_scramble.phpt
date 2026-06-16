--TEST--
mysqlnd sha256_password: reject server scramble shorter than 20 bytes
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
ob_start();
phpinfo(INFO_MODULES);
$tmp = ob_get_contents();
ob_end_clean();
if (!stristr($tmp, "auth_plugin_sha256_password"))
    die("skip SHA256 auth plugin not built-in to mysqlnd");
?>
--FILE--
<?php
require_once 'fake_server.inc';

mysqli_report(MYSQLI_REPORT_OFF);

$process = run_fake_server_in_background('sha256_password_short_scramble');
$process->wait();

$key_file = __DIR__ . '/mysqli_sha256_password_short_scramble.pem';
file_put_contents($key_file, <<<PEM
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvz0SP+XekztaRD5Ghdx8
ZLjXB8TKlF3tup4n6fJqhf6LyCX//AX/ppTZUmoiptxMucJrSaXYuI2YOeh2tfuR
HFMLqlAnK96UxpQMfWSSXVTKaeA4ld0DwQjQeWUIN+2AAN6HNGp7ilnS+7DlSHhd
7KLLZDJqrhQWtDynxGuIx55bAgPMp7adVn8CXg1vVMh62KYZDYmGU69HVVEuHq1n
odJdC1S2HYSJ4LgVIKg9eUBPKO1Bvwrwxse7UDGUNCSOyF+r8/jTKdrPdXp1qRNQ
9dU4/gMzMWd2NJ3+unXaLEEmpWEj/8x56X46PmpfbVAKm0tyn7YZH3tqamPo1piz
rQIDAQAB
-----END PUBLIC KEY-----
PEM);

$link = mysqli_init();
$link->options(MYSQLI_SERVER_PUBLIC_KEY, $key_file);
@$link->real_connect("127.0.0.1", "root", "password longer than the scramble", "", $process->getPort());
printf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

$process->terminate();
print "done!\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/mysqli_sha256_password_short_scramble.pem');
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
%A
[2027] The server sent wrong length for scramble
done!
