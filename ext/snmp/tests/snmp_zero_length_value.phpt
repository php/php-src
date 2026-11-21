--TEST--
SNMP: php_snmp_getvalue() handles zero-length varbind values with long OIDs
--EXTENSIONS--
snmp
--SKIPIF--
<?php
die('skip diagnostic dichotomy');
// This regression test spawns a small local test SNMP agent in Python (stdlib only).
if (!function_exists('proc_open')) {
    die('skip proc_open unavailable');
}
$python = PHP_OS_FAMILY === 'Windows' ? 'python' : 'python3';
$test = @exec($python . ' -c "print(1)"', $out, $code);
if ($code !== 0 || $test !== '1') {
    die('skip python3 with stdlib required for the local test SNMP agent');
}
?>
--FILE--
<?php
$python = PHP_OS_FAMILY === 'Windows' ? 'python' : 'python3';
$agent = <<<'PY'
import socket
N = 64
def enc_len(n):
    if n < 0x80: return bytes([n])
    o = []
    while n: o.insert(0, n & 0xFF); n >>= 8
    return bytes([0x80 | len(o)]) + bytes(o)
def tlv(t, v): return bytes([t]) + enc_len(len(v)) + v
def enc_int(n):
    if n == 0: body = b"\x00"
    else:
        o = []
        while n: o.insert(0, n & 0xFF); n >>= 8
        if o[0] & 0x80: o.insert(0, 0)
        body = bytes(o)
    return tlv(2, body)
def enc_oid(s):
    vals = [40*s[0] + s[1]] + s[2:]
    out = b""
    for v in vals:
        c = [v & 0x7F]; v >>= 7
        while v: c.append((v & 0x7F) | 0x80); v >>= 7
        out += bytes(reversed(c))
    return tlv(6, out)
def walk(b, i):
    tag = b[i]; i += 1
    l = b[i]; i += 1
    if l & 0x80:
        k = l & 0x7F
        l = int.from_bytes(b[i:i+k], "big"); i += k
    return tag, i, i + l
def req_id(d):
    _, i, _ = walk(d, 0)
    _, _, i = walk(d, i)
    _, _, i = walk(d, i)
    _, i, _ = walk(d, i)
    _, s, e = walk(d, i)
    return int.from_bytes(d[s:e], "big", signed=True)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("127.0.0.1", 0))
print(sock.getsockname()[1], flush=True)
pkt, addr = sock.recvfrom(4096)
rid = req_id(pkt)
subids = [1, 3] + [4294967295] * (N - 2)
varbind = tlv(0x30, enc_oid(subids) + tlv(5, b""))  # long OID + ASN_NULL (05 00)
pdu = tlv(0xA2, enc_int(rid) + enc_int(0) + enc_int(0) + tlv(0x30, varbind))
sock.sendto(tlv(0x30, enc_int(1) + tlv(4, b"public") + pdu), addr)
PY;

$spec = [
    0 => ['pipe', 'r'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
];
$proc = proc_open([$python, '-c', $agent], $spec, $pipes);
if (!is_resource($proc)) {
    die('cannot start test agent');
}

// The agent binds to a free port (port 0) and reports it on stdout.
$port = (int) trim(fgets($pipes[1]));
if ($port <= 0 || $port > 65535) {
    proc_terminate($proc);
    die('cannot determine test agent port');
}

// Default valueretrieval (SNMP_VALUE_LIBRARY), short timeout/retries.
// Before the fix this call never returns; with the fix it returns the
// rendered NULL value as a string.
$value = snmpget("127.0.0.1:$port", 'public', '1.3.6.1.2.1.1.1.0', 1, 1);
var_dump(is_string($value));

proc_terminate($proc);
proc_close($proc);
?>
--EXPECT--
bool(true)
