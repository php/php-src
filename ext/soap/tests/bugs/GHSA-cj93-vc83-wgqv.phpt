--TEST--
GHSA-cj93-vc83-wgqv
--INI--
soap.wsdl_cache_enabled=0
memory_limit=-1
--EXTENSIONS--
soap
--CONFLICTS--
all
--SKIPIF--
<?php
require __DIR__.'/../../../standard/tests/http/server.inc';
http_server_skipif();
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
if (PHP_INT_SIZE == 4) die('skip not for 32-bit systems due to memory constraints');

if (!file_exists('/proc/meminfo')) {
    die('skip Cannot check free RAM from /proc/meminfo on this platform');
}

/* The client allocates a ~4 GiB response buffer (a 2 GiB chunk plus the
 * oversized realloc for the second chunk), so require real headroom to avoid
 * swapping, which is what makes this test time out on constrained machines. */
$free_ram = 0;
if ($f = fopen("/proc/meminfo","r")) {
    while (!feof($f)) {
        if (preg_match('/MemFree[^\d]*(\d+)/i', fgets($f), $m)) {
            $free_ram = max($free_ram, $m[1]/1024/1024);
            if ($free_ram > 6) {
                $enough_free_ram = true;
            }
        }
    }
}

if (empty($enough_free_ram)) {
    die(sprintf("skip need +6G free RAM, but only %01.2f available", $free_ram));
}
--FILE--
<?php
require __DIR__.'/../../../standard/tests/http/server.inc';

function chunk_body($body, $n)
{
    $chunks = str_split($body, $n);
    $chunks[] = '';

    foreach ($chunks as $k => $v) {
        $chunks[$k] = sprintf("%08x\r\n%s\r\n", strlen($v), $v);
    }

    return join('', $chunks);
}

$wsdl = file_get_contents(__DIR__.'/../server030.wsdl');

$headers =
    "HTTP/1.1 200 OK\r\n".
    "Content-Type: text/xml;charset=utf-8\r\n".
    "Transfer-Encoding: \t  chunked\t \r\n".
    "Connection: close\r\n".
    "\r\n";

/* Custom minimal server. Unlike the generic http_server() helper it streams the
 * 2 GiB filler in bounded blocks instead of materialising it (and a data://
 * copy of it) in memory. That keeps the sender's footprint tiny: only the
 * client needs to hold the large buffers, so total memory and run time stay far
 * lower and the test no longer thrashes on slower machines. */
function heavy_soap_server($wsdl, $headers)
{
    $server = stream_socket_server('tcp://localhost:0', $errno, $errstr);
    if (!$server) {
        return false;
    }
    $uri = 'http://' . stream_socket_get_name($server, false);

    $pid = pcntl_fork();
    if ($pid == -1) {
        die('could not fork');
    } else if ($pid) {
        return ['pid' => $pid, 'uri' => $uri];
    }

    /* Child: streaming 2 GiB can exceed the 60s alarm the helper would use, so
     * match the run-tests per-test timeout instead. */
    pcntl_alarm(120);

    $drain = static function ($sock) {
        stream_set_blocking($sock, false);
        while (!feof($sock)) {
            $r = [$sock]; $w = $e = null;
            if (!stream_select($r, $w, $e, 1)) continue;
            $line = stream_get_line($sock, 8192, "\r\n");
            if ($line === '') break;
        }
        stream_set_blocking($sock, true);
    };

    /* Response 1: the WSDL, chunked. */
    $sock = stream_socket_accept($server, 60);
    if ($sock) {
        $drain($sock);
        fwrite($sock, $headers . chunk_body($wsdl, 64));
        fclose($sock);
    }

    /* Response 2: an oversized chunk. Only the size header of the second chunk
     * is needed: the reallocation for it happens before its body is read, so on
     * the unfixed code the overflow triggers on the first read into the
     * undersized buffer. The 2 GiB first-chunk body is streamed in 8 MiB blocks
     * rather than built as one string. */
    $sock = stream_socket_accept($server, 60);
    if ($sock) {
        $drain($sock);
        fwrite($sock, $headers);
        fwrite($sock, sprintf("%08x\r\n", 0x7fffffff));

        $remaining = 0x7fffffff;
        $block = str_repeat('x', 1 << 23); // 8 MiB
        $block_len = strlen($block);
        while ($remaining > 0) {
            $n = $remaining < $block_len ? $remaining : $block_len;
            fwrite($sock, $n === $block_len ? $block : substr($block, 0, $n));
            $remaining -= $n;
        }

        fwrite($sock, "\r\n");
        fwrite($sock, sprintf("%08x\r\n", 0x7fffffff));
        fwrite($sock, "xxxx");
        fclose($sock);
    }

    exit(0);
}

['pid' => $pid, 'uri' => $uri] = heavy_soap_server($wsdl, $headers);

$options = [
    'trace' => false,
    'location' => $uri,
];

$client = new SoapClient($uri, $options);

$client->getItems();

http_server_kill($pid);

--EXPECTF--
Fatal error: Uncaught SoapFault exception: [HTTP] Error Fetching http body, No Content-Length, connection closed or chunked data in %s:%d
Stack trace:
#0 [internal function]: SoapClient->__doRequest('<?xml version="...', '%s', 'http://testuri....', 1, false)
#1 %s(%d): SoapClient->__call('getItems', Array)
#2 {main}
  thrown in %s on line %d
