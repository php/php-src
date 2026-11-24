--TEST--
CURLOPT_DEBUGFUNCTION callback closing the CURLOPT_FILE stream must not leave a dangling FILE pointer
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$fp = fopen(__DIR__ . '/curl_debugfunction_fclose_file.tmp', 'w');

$ch = curl_init("{$host}/get.inc");
curl_setopt($ch, CURLOPT_FILE, $fp);
curl_setopt($ch, CURLOPT_VERBOSE, true);
curl_setopt($ch, CURLOPT_DEBUGFUNCTION, function (CurlHandle $ch, int $type, string $data) use ($fp) {
    if ($type === CURLINFO_HEADER_IN && is_resource($fp)) {
        fclose($fp);
    }
});

var_dump(curl_exec($ch));
echo "Done\n";
?>
--CLEAN--
<?php @unlink(__DIR__ . '/curl_debugfunction_fclose_file.tmp'); ?>
--EXPECTF--
Warning: curl_exec(): CURLOPT_FILE resource has gone away, resetting to default in %s on line %d
Hello World!
Hello World!bool(true)
Done
