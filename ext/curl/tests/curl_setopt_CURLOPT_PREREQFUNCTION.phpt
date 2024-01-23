--TEST--
Curl option CURLOPT_PREREQFUNCTION
--EXTENSIONS--
curl
--SKIPIF--
<?php
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x075000) die("skip: test works only with curl >= 7.80.0");
?>
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $host);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
//curl_setopt($ch, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

var_dump(CURLOPT_PREREQFUNCTION);
var_dump(CURL_PREREQFUNC_OK);
var_dump(CURL_PREREQFUNC_ABORT);

curl_setopt($ch, CURLOPT_PREREQFUNCTION, function() use ($port) {
	var_dump('callback');
	var_dump(func_num_args());
	$args = func_get_args();
	var_dump(get_class($args[0]));
	var_dump(filter_var($args[1], FILTER_VALIDATE_IP) !== false);
	var_dump(filter_var($args[2], FILTER_VALIDATE_IP) !== false);
	var_dump($port === $args[3]);
	var_dump(is_int($args[4]));

	return CURL_PREREQFUNC_ABORT;
});

$result = curl_exec($ch);

var_dump($result);
var_dump(curl_error($ch));
var_dump(curl_errno($ch));

var_dump('finished');
?>
--EXPECT--
int(20312)
int(0)
int(1)
string(8) "callback"
int(5)
string(10) "CurlHandle"
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
string(41) "operation aborted by pre-request callback"
int(42)
string(8) "finished"
