--TEST--
GH-14780: p(f)sockopen overflow on timeout.
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
$code = null;
$err = null;
try {
	pfsockopen('udp://127.0.0.1', '63844', $code, $err, (PHP_INT_MAX/100000)+1);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	pfsockopen('udp://127.0.0.1', '63844', $code, $err, (PHP_INT_MIN/100000)-1);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(pfsockopen('udp://127.0.0.1', '63844', $code, $err, -1));
try {
	pfsockopen('udp://127.0.0.1', '63844', $code, $err, NAN);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
	pfsockopen('udp://127.0.0.1', '63844', $code, $err, INF);
} catch (\Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
ValueError: pfsockopen(): Argument #6 must be -1 or between 0 and %s
ValueError: pfsockopen(): Argument #6 must be -1 or between 0 and %s
resource(%d) of type (persistent stream)
ValueError: pfsockopen(): Argument #6 must be -1 or between 0 and %s
ValueError: pfsockopen(): Argument #6 must be -1 or between 0 and %s
