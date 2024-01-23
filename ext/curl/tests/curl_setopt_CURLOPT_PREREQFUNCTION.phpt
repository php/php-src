--TEST--
Curl option CURLOPT_PREREQFUNCTION
--EXTENSIONS--
curl
filter
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
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);

$result = curl_exec($ch);

var_dump(CURLOPT_PREREQFUNCTION);
var_dump(CURL_PREREQFUNC_OK);
var_dump(CURL_PREREQFUNC_ABORT);

$returnValue = CURL_PREREQFUNC_ABORT;

echo "\nTesting with CURL_PREREQFUNC_ABORT\n";
$callback = function() use ($port, &$returnValue) {
	var_dump('callback');
	var_dump(func_num_args());
	$args = func_get_args();
	var_dump(get_class($args[0]));
	var_dump(filter_var($args[1], FILTER_VALIDATE_IP) !== false);
	var_dump(filter_var($args[2], FILTER_VALIDATE_IP) !== false);
	var_dump($port === $args[3]);
	var_dump(is_int($args[4]));

	return $returnValue;
};

curl_setopt($ch, CURLOPT_PREREQFUNCTION, $callback);

$result = curl_exec($ch);

var_dump($result);
var_dump(curl_error($ch));
var_dump(curl_errno($ch));

$returnValue = CURL_PREREQFUNC_OK;

echo "\nTesting with CURL_PREREQFUNC_OK\n";
$result = curl_exec($ch);

var_dump($result);
var_dump(curl_error($ch));
var_dump(curl_errno($ch));

echo "\nTesting with curl_copy_handle\n";
$ch2 = curl_copy_handle($ch);
$result = curl_exec($ch2);
var_dump($result);
var_dump(curl_error($ch2));
var_dump(curl_errno($ch2));

echo "\nTesting with no return type\n";
curl_setopt($ch, CURLOPT_PREREQFUNCTION, function() use ($port) {
	// returns nothing
});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\nTesting with invalid type\n";
curl_setopt($ch, CURLOPT_PREREQFUNCTION, function() use ($port) {
	return 'this should be an integer';
});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\nTesting with invalid value\n";
curl_setopt($ch, CURLOPT_PREREQFUNCTION, function() use ($port) {
	return 42;
});
try {
    curl_exec($ch);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\nTesting with invalid option value\n";
try {
    curl_setopt($ch, CURLOPT_PREREQFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\nTesting with invalid option callback\n";
try {
    curl_setopt($ch, CURLOPT_PREREQFUNCTION, 'function_does_not_exist');
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\nTesting with null as the callback\n";
var_dump(curl_setopt($ch, CURLOPT_PREREQFUNCTION, null));
var_dump(curl_exec($ch));
var_dump(curl_error($ch));
var_dump(curl_errno($ch));

echo "\nDone";
?>
--EXPECT--
int(20312)
int(0)
int(1)

Testing with CURL_PREREQFUNC_ABORT
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

Testing with CURL_PREREQFUNC_OK
string(8) "callback"
int(5)
string(10) "CurlHandle"
bool(true)
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
int(0)

Testing with curl_copy_handle
string(8) "callback"
int(5)
string(10) "CurlHandle"
bool(true)
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
int(0)

Testing with no return type
The CURLOPT_PREREQFUNCTION callback must return either CURL_PREREQFUNC_OK or CURL_PREREQFUNC_ABORT

Testing with invalid type
The CURLOPT_PREREQFUNCTION callback must return either CURL_PREREQFUNC_OK or CURL_PREREQFUNC_ABORT

Testing with invalid value
The CURLOPT_PREREQFUNCTION callback must return either CURL_PREREQFUNC_OK or CURL_PREREQFUNC_ABORT

Testing with invalid option value
curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_PREREQFUNCTION, no array or string given

Testing with invalid option callback
curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_PREREQFUNCTION, function "function_does_not_exist" not found or invalid function name

Testing with null as the callback
bool(true)
string(0) ""
string(0) ""
int(0)

Done
