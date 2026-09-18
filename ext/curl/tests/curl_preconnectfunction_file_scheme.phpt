--TEST--
CURLOPT_PRECONNECTFUNCTION is not called for file://, which creates no socket
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!in_array('file', curl_version()['protocols'], true)) die('skip file:// protocol not supported');
?>
--FILE--
<?php
$file = __DIR__ . '/curl_preconnectfunction_file_scheme.txt';
file_put_contents($file, "not a socket\n");

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, 'file://' . $file);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

$calls = 0;
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () use (&$calls): bool {
    $calls++;
    return false;
});

/* The callback refuses everything, yet the file is still read: the option is not
 * on its own an SSRF defence, CURLOPT_PROTOCOLS_STR must be used alongside it. */
var_dump(curl_exec($ch));
var_dump($calls);

curl_setopt($ch, CURLOPT_PROTOCOLS_STR, 'http,https');
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_UNSUPPORTED_PROTOCOL);

echo "Done";
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/curl_preconnectfunction_file_scheme.txt');
?>
--EXPECT--
string(13) "not a socket
"
int(0)
bool(false)
bool(true)
Done
