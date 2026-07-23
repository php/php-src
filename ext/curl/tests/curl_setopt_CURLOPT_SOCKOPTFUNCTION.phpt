--TEST--
Curl option CURLOPT_SOCKOPTFUNCTION
--EXTENSIONS--
curl
sockets
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

var_dump(CURL_SOCKOPT_OK);
var_dump(CURL_SOCKOPT_ERROR);
var_dump(CURL_SOCKOPT_ALREADY_CONNECTED);

$url = "{$host}/get.inc";

$seen = null;
$returnValue = CURL_SOCKOPT_OK;
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_SOCKOPTFUNCTION, function ($ch, $socket, $purpose) use (&$seen, &$returnValue) {
    // Capture once; the callback may run per connection attempt (IPv4/IPv6).
    $seen ??= [
        'handle' => $ch instanceof CurlHandle,
        'socket' => $socket instanceof Socket,
        'purpose' => $purpose === CURLSOCKTYPE_IPCXN,
        // The Socket wraps the live descriptor: socket options can be set on it.
        'set_option' => socket_set_option($socket, SOL_SOCKET, SO_KEEPALIVE, 1),
    ];
    return $returnValue;
});

echo "\nTesting with CURL_SOCKOPT_OK\n";
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));
var_dump($seen);

echo "\nTesting with CURL_SOCKOPT_ERROR\n";
$returnValue = CURL_SOCKOPT_ERROR;
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) !== 0);

echo "\nTesting with CURL_SOCKOPT_ALREADY_CONNECTED\n";
// Tells libcurl to skip the connect step. The transfer fails afterwards
// because the socket is not actually connected, which is the expected libcurl
// behaviour — what we are checking here is that the constant flows through.
$returnValue = CURL_SOCKOPT_ALREADY_CONNECTED;
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) !== 0);

echo "\nTesting with curl_copy_handle\n";
$returnValue = CURL_SOCKOPT_OK;
$ch2 = curl_copy_handle($ch);
var_dump(curl_exec($ch2));

echo "\nTesting with invalid return type\n";
curl_setopt($ch, CURLOPT_SOCKOPTFUNCTION, function ($ch, $socket, $purpose) {
    return 'not an int';
});
try {
    curl_exec($ch);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with invalid return value\n";
curl_setopt($ch, CURLOPT_SOCKOPTFUNCTION, function ($ch, $socket, $purpose) {
    return 42;
});
try {
    curl_exec($ch);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with invalid option value\n";
try {
    curl_setopt($ch, CURLOPT_SOCKOPTFUNCTION, 42);
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "\nTesting with null as the callback\n";
var_dump(curl_setopt($ch, CURLOPT_SOCKOPTFUNCTION, null));
var_dump(curl_exec($ch));
var_dump(curl_errno($ch));

echo "\nTesting that curl_reset() detaches the callback\n";
// curl_reset() is expected to drop every userland callback installed on the
// handle. Reusing the handle afterwards must not re-enter the previous closure.
$resetCount = 0;
$chReset = curl_init();
curl_setopt($chReset, CURLOPT_URL, $url);
curl_setopt($chReset, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($chReset, CURLOPT_SOCKOPTFUNCTION, function () use (&$resetCount) {
    $resetCount++;
    return CURL_SOCKOPT_OK;
});
curl_exec($chReset);
$before = $resetCount;
curl_reset($chReset);
curl_setopt($chReset, CURLOPT_URL, $url);
curl_setopt($chReset, CURLOPT_RETURNTRANSFER, 1);
curl_exec($chReset);
var_dump($resetCount === $before);

echo "\nTesting socket_close() from within the callback\n";
// A defensive callback may decide to abort by closing the socket itself; the
// extension must not double-close or use-after-free. The transfer fails as
// expected (libcurl tries to use the now-closed descriptor) but the engine
// stays consistent.
$ch4 = curl_init();
curl_setopt($ch4, CURLOPT_URL, $url);
curl_setopt($ch4, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch4, CURLOPT_SOCKOPTFUNCTION, function ($ch, $socket, $purpose) {
    socket_close($socket);
    return CURL_SOCKOPT_ERROR;
});
var_dump(curl_exec($ch4));
var_dump(curl_errno($ch4) !== 0);

echo "\nTesting a callback that throws aborts the connection\n";
// On the exception path, retval is undef. The extension translates that into
// CURL_SOCKOPT_ERROR so libcurl aborts the connection instead of carrying on
// with a half-configured socket — consistent with opensocket / ssh_hostkey.
$ch5 = curl_init();
curl_setopt($ch5, CURLOPT_URL, $url);
curl_setopt($ch5, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch5, CURLOPT_SOCKOPTFUNCTION, function ($ch, $socket, $purpose) {
    throw new LogicException('abort');
});
try {
    curl_exec($ch5);
    echo "FAIL: curl_exec returned without an exception\n";
} catch (\LogicException $e) {
    echo "caught: ", $e->getMessage(), \PHP_EOL;
}
// After the exception, the handle's last errno reflects the abort.
var_dump(curl_errno($ch5) !== 0);

echo "\nTesting low-level tuning from the callback\n";
// The callback can tune the socket through any ext/sockets API. libcurl is
// robust to userland touching TCP_NODELAY and the blocking mode — it
// reapplies whatever it needs once the callback returns.
$tunings = null;
$ch3 = curl_init();
curl_setopt($ch3, CURLOPT_URL, $url);
curl_setopt($ch3, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch3, CURLOPT_TCP_NODELAY, 1);
curl_setopt($ch3, CURLOPT_SOCKOPTFUNCTION, function ($ch, $socket, $purpose) use (&$tunings) {
    $tunings ??= [
        'tcp_nodelay_off' => socket_set_option($socket, SOL_TCP, TCP_NODELAY, 0),
        'set_block'       => socket_set_block($socket),
        'set_nonblock'    => socket_set_nonblock($socket),
    ];
    return CURL_SOCKOPT_OK;
});
var_dump(curl_exec($ch3));
var_dump(curl_errno($ch3));
var_dump($tunings);

echo "\nDone";
?>
--EXPECT--
int(0)
int(1)
int(2)

Testing with CURL_SOCKOPT_OK
string(25) "Hello World!
Hello World!"
int(0)
array(4) {
  ["handle"]=>
  bool(true)
  ["socket"]=>
  bool(true)
  ["purpose"]=>
  bool(true)
  ["set_option"]=>
  bool(true)
}

Testing with CURL_SOCKOPT_ERROR
bool(false)
bool(true)

Testing with CURL_SOCKOPT_ALREADY_CONNECTED
bool(false)
bool(true)

Testing with curl_copy_handle
string(25) "Hello World!
Hello World!"

Testing with invalid return type
The CURLOPT_SOCKOPTFUNCTION callback must return one of CURL_SOCKOPT_OK, CURL_SOCKOPT_ERROR or CURL_SOCKOPT_ALREADY_CONNECTED

Testing with invalid return value
The CURLOPT_SOCKOPTFUNCTION callback must return one of CURL_SOCKOPT_OK, CURL_SOCKOPT_ERROR or CURL_SOCKOPT_ALREADY_CONNECTED

Testing with invalid option value
curl_setopt(): Argument #3 ($value) must be a valid callback for option CURLOPT_SOCKOPTFUNCTION, no array or string given

Testing with null as the callback
bool(true)
string(25) "Hello World!
Hello World!"
int(0)

Testing that curl_reset() detaches the callback
bool(true)

Testing socket_close() from within the callback
bool(false)
bool(true)

Testing a callback that throws aborts the connection
caught: abort
bool(true)

Testing low-level tuning from the callback
string(25) "Hello World!
Hello World!"
int(0)
array(3) {
  ["tcp_nodelay_off"]=>
  bool(true)
  ["set_block"]=>
  bool(true)
  ["set_nonblock"]=>
  bool(true)
}

Done
