--TEST--
GHSA-www2-q4fc-65wf
--DESCRIPTION--
This is a ZPP test but *keep* this as it is security-sensitive!
--FILE--
<?php
try {
    dns_check_record("\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    dns_get_mx("\0", $out);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    dns_get_record("\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    getprotobyname("\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    getservbyname("\0", "tcp");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    getservbyname("x", "tcp\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    getservbyport(0, "tcp\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    inet_pton("\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    ip2long("\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: dns_check_record(): Argument #1 ($hostname) must not contain any null bytes
ValueError: dns_get_mx(): Argument #1 ($hostname) must not contain any null bytes
ValueError: dns_get_record(): Argument #1 ($hostname) must not contain any null bytes
ValueError: getprotobyname(): Argument #1 ($protocol) must not contain any null bytes
ValueError: getservbyname(): Argument #1 ($service) must not contain any null bytes
ValueError: getservbyname(): Argument #2 ($protocol) must not contain any null bytes
ValueError: getservbyport(): Argument #2 ($protocol) must not contain any null bytes
ValueError: inet_pton(): Argument #1 ($ip) must not contain any null bytes
ValueError: ip2long(): Argument #1 ($ip) must not contain any null bytes
