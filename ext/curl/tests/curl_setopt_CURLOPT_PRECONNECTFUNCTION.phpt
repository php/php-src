--TEST--
Curl option CURLOPT_PRECONNECTFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

/* libcurl invokes the callback once per socket it creates, so a dual stack host
 * may be attempted for more than one address family. Every assertion below is
 * therefore made over all the attempts, never on their number. */
function all_handles_are(array $seen, CurlHandle $expected): bool {
    if ($seen === []) {
        return false;
    }
    foreach ($seen as $handle) {
        if ($handle !== $expected) {
            return false;
        }
    }
    return true;
}

var_dump(CURLOPT_PRECONNECTFUNCTION);

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

echo "\nAllowing the connection\n";
$seen = [];
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function (CurlHandle $handle, ?string $ip, int $port, CurlAddressFamily $family) use (&$seen): bool {
    $seen[] = [$ip, $port, $family];
    return true;
});
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));
/* The test server listens on localhost, so whichever family libcurl tries the
 * address is one of the two loopback literals. Checked without ext/filter so that
 * curl alone is enough to run this test. */
$ok = $seen !== [];
foreach ($seen as [$ip, $p, $family]) {
    $ok = $ok
        && in_array($ip, ['127.0.0.1', '::1'], true)
        && $p === $port
        && $family === ($ip === '::1' ? CurlAddressFamily::Inet6 : CurlAddressFamily::Inet);
}
var_dump($ok);

echo "\nRefusing the connection\n";
$refuse = function (): bool { return false; };
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, $refuse);
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "\nTesting with curl_copy_handle\n";
/* The copy must be given its own callback, invoked with the copy. */
$invoked = [];
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function (CurlHandle $handle) use (&$invoked): bool {
    $invoked[] = $handle;
    return true;
});
var_dump(curl_exec($ch));
var_dump(all_handles_are($invoked, $ch));
$ch2 = curl_copy_handle($ch);
$invoked = [];
var_dump(curl_exec($ch2));
var_dump(all_handles_are($invoked, $ch2));
var_dump(curl_errno($ch2));
curl_setopt($ch2, CURLOPT_PRECONNECTFUNCTION, $refuse);
var_dump(curl_exec($ch2));
var_dump(curl_errno($ch2) === CURLE_COULDNT_CONNECT);
unset($ch2);

echo "\nTesting with curl_reset\n";
$ch3 = curl_copy_handle($ch);
$cb3 = function (): bool { return false; };
curl_setopt($ch3, CURLOPT_PRECONNECTFUNCTION, $cb3);
$weak = WeakReference::create($cb3);
unset($cb3);
curl_reset($ch3);
/* curl_reset() must release the callback, not merely clear the libcurl option. */
var_dump($weak->get() === null);
curl_setopt($ch3, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($ch3, CURLOPT_RETURNTRANSFER, true);
var_dump(curl_exec($ch3));
unset($ch3);

echo "\nTesting with no return value\n";
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () {});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting with an invalid return type\n";
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () { return 1; });
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting with an exception thrown from the callback\n";
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, function () {
    throw new \Exception('refused');
});
try {
    curl_exec($ch);
} catch (\Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "\nTesting with an invalid option value\n";
try {
    curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting with an invalid option callback\n";
try {
    curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, 'function_does_not_exist');
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting through curl_setopt_array()\n";
try {
    curl_setopt_array($ch, [CURLOPT_PRECONNECTFUNCTION => 42]);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting a rejected callable followed by curl_copy_handle()\n";
$src = curl_init();
curl_setopt($src, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($src, CURLOPT_RETURNTRANSFER, true);
curl_setopt($src, CURLOPT_PRECONNECTFUNCTION, $refuse);
try {
    curl_setopt($src, CURLOPT_PRECONNECTFUNCTION, 'function_does_not_exist');
} catch (\TypeError $e) {
    echo $e::class, PHP_EOL;
}
/* A rejected callable also releases the previously installed handler, so the
 * option is left disarmed and libcurl creates the socket itself again. The copy
 * must not carry a CURLOPT_OPENSOCKETDATA pointer to the source handle, which
 * would dangle as soon as the source is released. */
$copy = curl_copy_handle($src);
unset($src);
var_dump(curl_exec($copy));
unset($copy);

echo "\nTesting with null as the callback\n";
$cbn = function (): bool { return false; };
curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, $cbn);
$weakNull = WeakReference::create($cbn);
unset($cbn);
var_dump(curl_exec($ch));
var_dump(curl_setopt($ch, CURLOPT_PRECONNECTFUNCTION, null));
/* null must release the callback and hand socket creation back to libcurl. */
var_dump($weakNull->get() === null);
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));

echo "\nDone";
?>
--EXPECT--
int(19915)

Allowing the connection
string(25) "Hello World!
Hello World!"
int(0)
bool(true)

Refusing the connection
bool(false)
bool(true)

Testing with curl_copy_handle
string(25) "Hello World!
Hello World!"
bool(true)
string(25) "Hello World!
Hello World!"
bool(true)
int(0)
bool(false)
bool(true)

Testing with curl_reset
bool(true)
string(25) "Hello World!
Hello World!"

Testing with no return value
TypeError: The CURLOPT_PRECONNECTFUNCTION callback must return a bool

Testing with an invalid return type
TypeError: The CURLOPT_PRECONNECTFUNCTION callback must return a bool

Testing with an exception thrown from the callback
Exception: refused
bool(true)

Testing with an invalid option value
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_PRECONNECTFUNCTION, no array or string given

Testing with an invalid option callback
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_PRECONNECTFUNCTION, function "function_does_not_exist" not found or invalid function name

Testing through curl_setopt_array()
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_PRECONNECTFUNCTION, no array or string given

Testing a rejected callable followed by curl_copy_handle()
TypeError
string(25) "Hello World!
Hello World!"

Testing with null as the callback
bool(false)
bool(true)
bool(true)
string(25) "Hello World!
Hello World!"
int(0)

Done
