--TEST--
Curl option CURLOPT_DEBUGFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$allowedTypes = [
    CURLINFO_TEXT,
    CURLINFO_HEADER_IN,
    CURLINFO_HEADER_OUT,
    CURLINFO_DATA_IN,
    CURLINFO_DATA_OUT,
    CURLINFO_SSL_DATA_OUT,
    CURLINFO_SSL_DATA_IN,
];

var_dump(CURLOPT_DEBUGFUNCTION);

$host = curl_cli_server_start();

echo "\n===Testing with regular CURLOPT_VERBOSE with verbose=false===\n";
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_VERBOSE, 0);
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, function() {
    echo 'This should not be called';
}));
curl_exec($ch);
curl_setopt($ch, CURLOPT_VERBOSE, 1);

echo "\n===Testing with regular CURLOPT_VERBOSE on stderr===\n";
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_VERBOSE, 1);
$stderr = fopen('php://temp', 'wb+');
curl_setopt($ch, CURLOPT_STDERR, $stderr);
$curlVerboseOutput = curl_exec($ch);
rewind($stderr);
$receivedOutput = stream_get_contents($stderr);
fclose($stderr);
var_dump(str_contains($receivedOutput, 'Host'));

echo "\n===Testing with CURLOPT_DEBUGFUNCTION happy path===\n";
$stderr = fopen('php://temp', 'wb+');
curl_setopt($ch, CURLOPT_STDERR, $stderr);

$debugOutput = [];
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, function(CurlHandle $curlHandle, int $type, string $data) use ($allowedTypes, &$debugOutput) {
    if (!in_array($type, $allowedTypes)) {
        throw new Exception('Unexpected type value: '. $type);
    }
    $debugOutput[$type][] = $data;
}));

$result = curl_exec($ch);
rewind($stderr);
$receivedOutputWithDebugFunction = stream_get_contents($stderr);
fclose($stderr);
echo "Received stderr empty:\n";
var_dump($result);
var_dump($receivedOutputWithDebugFunction);

// Header-out should be an exact match
var_dump(str_contains($receivedOutput, $debugOutput[CURLINFO_HEADER_OUT][0]));

// Header-in fields should match, except for the "Date" header that is dynamic.
foreach ($debugOutput[CURLINFO_HEADER_IN] as $headerReceived) {
    if (str_starts_with($headerReceived, 'Date')) {
        continue;
    }
    if (!str_contains($receivedOutput, $headerReceived)) {
        throw new \Exception('DEBUGFUNCTION header field does not match the previous verbose debug message');
    }
}

echo "\n===Test attempting to set CURLINFO_HEADER_OUT while CURLOPT_DEBUGFUNCTION in effect throws===\n";
$ch = curl_init();
var_dump(curl_setopt($ch, CURLINFO_HEADER_OUT, true));
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, null));
var_dump(curl_setopt($ch, CURLINFO_HEADER_OUT, true));
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, 'strlen'));
try {
    var_dump(curl_setopt($ch, CURLINFO_HEADER_OUT, true));
}
catch (\ValueError $e) {
    var_dump($e->getMessage());
}
$chCopy = curl_copy_handle($ch);
try {
    var_dump(curl_setopt($chCopy, CURLINFO_HEADER_OUT, true));
}
catch (\ValueError $e) {
    var_dump($e->getMessage());
}
var_dump(curl_setopt($chCopy, CURLOPT_DEBUGFUNCTION, null));
var_dump(curl_setopt($chCopy, CURLINFO_HEADER_OUT, true));

echo "\n===Test attempting to set CURLOPT_DEBUGFUNCTION while CURLINFO_HEADER_OUT does not throw===\n";
$ch = curl_init();
var_dump(curl_setopt($ch, CURLINFO_HEADER_OUT, true));
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, null));
var_dump(curl_setopt($ch, CURLINFO_HEADER_OUT, true));
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, 'strlen'));
$chCopy = curl_copy_handle($ch);
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, 'strlen'));
var_dump(curl_setopt($chCopy, CURLOPT_DEBUGFUNCTION, null));
var_dump(curl_setopt($chCopy, CURLINFO_HEADER_OUT, 1));

echo "\n===Test CURLOPT_DEBUGFUNCTION=null works===\n";
$ch = curl_init("{$host}/get.inc?test=file");
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, null));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
var_dump(curl_exec($ch));

echo "\n===Test CURLINFO_HEADER_OUT works while CURLOPT_DEBUGFUNCTION in effect===\n";
$ch = curl_init("{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
var_dump(curl_getinfo($ch, CURLINFO_HEADER_OUT));
var_dump(curl_setopt($ch, CURLINFO_HEADER_OUT, true));
var_dump(curl_getinfo($ch, CURLINFO_HEADER_OUT));
var_dump(curl_setopt($ch, CURLOPT_DEBUGFUNCTION, static function() {}));
var_dump(curl_getinfo($ch, CURLINFO_HEADER_OUT));
var_dump($result = curl_exec($ch));
var_dump(curl_getinfo($ch, CURLINFO_HEADER_OUT));

echo "\n===Test CURLOPT_DEBUGFUNCTION can throw within callback===\n";
$ch = curl_init("{$host}/get.inc?test=file");
curl_setopt($ch, CURLOPT_DEBUGFUNCTION, static function() {
    throw new \RuntimeException('This should get caught after verbose=true');
});
var_dump(curl_exec($ch));
curl_setopt($ch, CURLOPT_VERBOSE, true);
try {
    var_dump($result = curl_exec($ch));
}
catch (\RuntimeException $e) {
    var_dump($e->getMessage());
}
var_dump(curl_getinfo($ch, CURLINFO_HEADER_OUT));

echo "\n===Test CURLOPT_DEBUGFUNCTION on shared handles work===\n";
$ch = curl_init("{$host}/get.inc?test=file");
$called = false;
curl_setopt_array($ch, [
    CURLOPT_VERBOSE => true,
    CURLOPT_DEBUGFUNCTION => static function() use (&$called) {
        $called = true;
    },
]);
var_dump($called);
curl_exec($ch);
var_dump($called);
$called = false;
$ch2 = curl_copy_handle($ch);
curl_exec($ch2);
var_dump($called);
var_dump(curl_getinfo($ch2, CURLINFO_HEADER_OUT));

echo "\nDone";
?>
--EXPECTF--
int(20094)

===Testing with regular CURLOPT_VERBOSE with verbose=false===
bool(true)

===Testing with regular CURLOPT_VERBOSE on stderr===
bool(true)

===Testing with CURLOPT_DEBUGFUNCTION happy path===
bool(true)
Received stderr empty:
string(0) ""
string(0) ""
bool(true)

===Test attempting to set CURLINFO_HEADER_OUT while CURLOPT_DEBUGFUNCTION in effect throws===
bool(true)
bool(true)
bool(true)
bool(true)
string(87) "CURLINFO_HEADER_OUT option must not be set when the CURLOPT_DEBUGFUNCTION option is set"
string(87) "CURLINFO_HEADER_OUT option must not be set when the CURLOPT_DEBUGFUNCTION option is set"
bool(true)
bool(true)

===Test attempting to set CURLOPT_DEBUGFUNCTION while CURLINFO_HEADER_OUT does not throw===
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

===Test CURLOPT_DEBUGFUNCTION=null works===
bool(true)
string(0) ""

===Test CURLINFO_HEADER_OUT works while CURLOPT_DEBUGFUNCTION in effect===
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
string(0) ""
string(%d) "GET /get.inc?test=file HTTP/%s
Host: %s:%d
Accept: */*

"

===Test CURLOPT_DEBUGFUNCTION can throw within callback===
bool(true)
string(41) "This should get caught after verbose=true"
string(%d) "GET /get.inc?test=file HTTP/%s
Host: %s:%d
Accept: */*

"

===Test CURLOPT_DEBUGFUNCTION on shared handles work===
bool(false)
bool(true)
bool(true)
string(71) "GET /get.inc?test=file HTTP/%s
Host: %s:%d
Accept: */*

"

Done
