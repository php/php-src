--TEST--
file upload greater than 2G
--SKIPIF--
<?php
include "skipif.inc";

if (PHP_INT_SIZE < 8) {
	die("skip need PHP_INT_SIZE>=8");
}

if ($f = fopen("/proc/meminfo","r")) {
	while (!feof($f)) {
		if (!strncmp($line = fgets($f), "MemFree", 7)) {
			if (substr($line,8)/1024/1024 > 3) {
				$enough_free_ram = true;
			}
		}
	}
}

if (empty($enough_free_ram)) {
	die("skip need +3G free RAM");
}

if (getenv('TRAVIS')) {
    die("skip Fails intermittently on travis");
}
?>
--FILE--
<?php

echo "Test\n";

include "php_cli_server.inc";

php_cli_server_start("var_dump(\$_FILES);", false,
	"-d post_max_size=3G -d upload_max_filesize=3G");

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;
$length = 2150000000;
$output = "";

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

$prev = "----123
Content-Type: text/plain; charset=UTF-8
Content-Disposition: form-data; name=\"file1\"; filename=\"file1.txt\"\n\n";
$post = "\n----123--\n";
$total = $length + strlen($prev) + strlen($post);

fwrite($fp, <<<EOF
POST /index.php HTTP/1.1
Host: {$host}
Content-Type: multipart/form-data; boundary=--123
Content-Length: {$total}

{$prev}
EOF
) or die("write prev failed");

$data = str_repeat("0123456789", 10000);
for ($i = 0; $i < $length; $i += 10000 * 10) {
	fwrite($fp, $data) or die("write failed @ ($i)");
}

fwrite($fp, $post) or die("write post failed");

while (!feof($fp)) {
	$output .= fgets($fp);
}
echo $output;
fclose($fp);
?>
Done
--EXPECTF--
Test

HTTP/1.1 200 OK
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

array(1) {
  ["file1"]=>
  array(5) {
    ["name"]=>
    string(9) "file1.txt"
    ["type"]=>
    string(10) "text/plain"
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(2150000000)
  }
}
Done
