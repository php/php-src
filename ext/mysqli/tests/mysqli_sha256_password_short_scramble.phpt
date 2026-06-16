--TEST--
mysqlnd sha256_password: reject server scramble shorter than 20 bytes
--EXTENSIONS--
mysqli
mysqlnd
openssl
--FILE--
<?php
require_once 'fake_server.inc';

mysqli_report(MYSQLI_REPORT_OFF);

$process = run_fake_server_in_background('sha256_password_short_scramble');
$process->wait();

$link = @new mysqli("127.0.0.1", "root", "", "", $process->getPort());
printf("[%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

$process->terminate();
print "done!\n";
?>
--EXPECTF--
[*] Server started on 127.0.0.1:%d
%A
[2027] The server sent wrong length for scramble
done!
