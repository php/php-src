--TEST--
curl with expect 100-continue against PHP development server
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

// Generate a POST body larger than 1MB to trigger Expect: 100-continue
$body = str_repeat('A', 1024 * 1024 + 1);

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=input");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_POSTFIELDS, $body);

// Set a high timeout for 100-continue response
curl_setopt($ch, CURLOPT_EXPECT_100_TIMEOUT_MS, 2000);

curl_exec($ch);
var_dump(curl_errno($ch));

echo "Did the PHP development server send a HTTP/1.1 100 Continue header?\n";
$start_transfer_time = curl_getinfo($ch, CURLINFO_STARTTRANSFER_TIME_T);
var_dump($start_transfer_time < 1_000_000);
?>
--EXPECT--
int(0)
Did the PHP development server send a HTTP/1.1 100 Continue header?
bool(true)
