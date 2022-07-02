--TEST--
Bug #48203 (Crash when CURLOPT_STDERR is set to regular file)
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$fp = fopen(__DIR__ . '/bug48203.tmp', 'w');

$ch = curl_init();

curl_setopt($ch, CURLOPT_VERBOSE, 1);
curl_setopt($ch, CURLOPT_STDERR, $fp);
curl_setopt($ch, CURLOPT_URL, curl_cli_server_start());

fclose($fp); // <-- premature close of $fp caused a crash!

curl_exec($ch);
curl_close($ch);

echo "Ok\n";

?>
--CLEAN--
<?php @unlink(__DIR__ . '/bug48203.tmp'); ?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_STDERR resource has gone away, resetting to stderr in %s on line %d
%A
Hello World!
Hello World!%A
Ok
