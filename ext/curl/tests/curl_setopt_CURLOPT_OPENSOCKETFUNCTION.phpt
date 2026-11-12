--TEST--
Curl option CURLOPT_OPENSOCKETFUNCTION
--EXTENSIONS--
curl
--FILE--
<?php
include 'server.inc';

$host = curl_cli_server_start();
$port = (int) (explode(':', $host))[1];

/* libcurl calls the callback once per address it tries, so a dual stack host may
 * be tried for both families. Checks are made over all calls, not on their number. */
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

var_dump(CURLOPT_OPENSOCKETFUNCTION);

$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

echo "\nAllowing the connection\n";
$seen = [];
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle, string $address, int $port) use (&$seen): bool {
    $seen[] = [$address, $port];
    return true;
});
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));
$ok = $seen !== [];
foreach ($seen as [$address, $p]) {
    $ok = $ok && in_array($address, ['127.0.0.1', '::1'], true) && $p === $port;
}
var_dump($ok);

echo "\nRefusing the connection\n";
$refuse = function (): bool { return false; };
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, $refuse);
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "\nTesting with curl_copy_handle\n";
$invoked = [];
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle) use (&$invoked): bool {
    $invoked[] = $handle;
    return true;
});
var_dump(curl_exec($ch));
var_dump(all_handles_are($invoked, $ch));
$ch2 = curl_copy_handle($ch);
$invoked = [];
var_dump(curl_exec($ch2));
var_dump(all_handles_are($invoked, $ch2));
curl_setopt($ch2, CURLOPT_OPENSOCKETFUNCTION, $refuse);
var_dump(curl_exec($ch2));
var_dump(curl_errno($ch2) === CURLE_COULDNT_CONNECT);
unset($ch2);

echo "\nTesting with curl_reset\n";
$ch3 = curl_copy_handle($ch);
$cb3 = function (): bool { return false; };
curl_setopt($ch3, CURLOPT_OPENSOCKETFUNCTION, $cb3);
$weak = WeakReference::create($cb3);
unset($cb3);
curl_reset($ch3);
var_dump($weak->get() === null);
curl_setopt($ch3, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($ch3, CURLOPT_RETURNTRANSFER, true);
var_dump(curl_exec($ch3));
unset($ch3);

echo "\nTesting with no return value\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function () {});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting with an invalid return type\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function () { return 1; });
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump(curl_errno($ch) === CURLE_COULDNT_CONNECT);

echo "\nTesting with an exception thrown from the callback\n";
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function () {
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
    curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting through curl_setopt_array()\n";
try {
    curl_setopt_array($ch, [CURLOPT_OPENSOCKETFUNCTION => 'function_does_not_exist']);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "\nTesting a rejected callable followed by curl_copy_handle()\n";
$src = curl_init();
curl_setopt($src, CURLOPT_URL, "{$host}/get.inc");
curl_setopt($src, CURLOPT_RETURNTRANSFER, true);
curl_setopt($src, CURLOPT_OPENSOCKETFUNCTION, $refuse);
try {
    curl_setopt($src, CURLOPT_OPENSOCKETFUNCTION, 'function_does_not_exist');
} catch (\TypeError $e) {
    echo $e::class, PHP_EOL;
}
/* A rejected callable releases the previous one, like for other callback options */
$copy = curl_copy_handle($src);
unset($src);
var_dump(curl_exec($copy));
unset($copy);

echo "\nTesting with null as the callback\n";
$cbn = function (): bool { return false; };
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, $cbn);
$weakNull = WeakReference::create($cbn);
unset($cbn);
var_dump(curl_exec($ch));
var_dump(curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, null));
var_dump($weakNull->get() === null);
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));

echo "\nTesting a callback that removes itself\n";
$calls = 0;
curl_setopt($ch, CURLOPT_OPENSOCKETFUNCTION, function (CurlHandle $handle) use (&$calls): bool {
    $calls++;
    curl_setopt($handle, CURLOPT_OPENSOCKETFUNCTION, null);
    return true;
});
var_dump(curl_exec($ch));
var_dump(curl_exec($ch));
var_dump($calls);

echo "\nDone";
?>
--EXPECT--
int(20163)

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
bool(false)
bool(true)

Testing with curl_reset
bool(true)
string(25) "Hello World!
Hello World!"

Testing with no return value
TypeError: The CURLOPT_OPENSOCKETFUNCTION callback must return a bool

Testing with an invalid return type
TypeError: The CURLOPT_OPENSOCKETFUNCTION callback must return a bool
bool(true)

Testing with an exception thrown from the callback
Exception: refused
bool(true)

Testing with an invalid option value
TypeError: curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_OPENSOCKETFUNCTION, no array or string given

Testing through curl_setopt_array()
TypeError: curl_setopt_array(): Argument #2 ($options) must be a valid callback for option CURLOPT_OPENSOCKETFUNCTION, function "function_does_not_exist" not found or invalid function name

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

Testing a callback that removes itself
string(25) "Hello World!
Hello World!"
string(25) "Hello World!
Hello World!"
int(1)

Done
