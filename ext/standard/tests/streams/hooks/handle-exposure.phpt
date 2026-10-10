--TEST--
IO hooks: a stream's handle is one object, created on demand, exposed only for a stream a script holds
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $seen = [];
    public ?StreamPollWeakHandle $inner = null;

    public function run(\Io\Operation $op): \Io\Completion
    {
        $handle = $op->getHandle();
        if ($handle instanceof StreamPollWeakHandle) {
            $key = $handle->isExposed() ? 'exposed' : 'internal';
            // Same object for the same stream, never the stream while the op runs
            $this->seen[$key] = ($this->seen[$key] ?? true)
                && $op->getHandle() === $handle && $handle->getStream() === null && $handle->isValid();
            if (!$handle->isExposed()) {
                $this->inner = $handle;
            }
        }
        return parent::run($op);
    }
}

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

// The factory returns the stream's one handle, exposed
$h = StreamPollWeakHandle::create($a);
var_dump($h === StreamPollWeakHandle::create($a), $h->isExposed(), $h->isValid(), $h->getStream() === $a);

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($a, $h, $server) {
    // The op on $a carries $h; the ops on the wrapper's inner socket carry an internal handle
    var_dump(fread($a, 10));
    $conn = stream_socket_accept($server, 5);
    while (!str_ends_with($request ?? '', "\r\n\r\n")) {
        $request = ($request ?? '') . fread($conn, 1024);
    }
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 2\r\n\r\nok");
    fclose($conn);
});
$scheduler->spawn(function () use ($b, $addr) {
    fwrite($b, "hello");
    var_dump(file_get_contents("http://$addr/"));
});
$scheduler->loop();
Io\Hooks\set_hooks(null);

var_dump($scheduler->seen);
// The inner socket is closed: its handle answers invalid and hands nothing out
var_dump($scheduler->inner->isValid(), $scheduler->inner->isExposed(), $scheduler->inner->getStream());

// The handle outlives its stream as an invalid one
fclose($a);
var_dump($h->isValid(), $h->getStream(), $h->isExposed());
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(5) "hello"
string(2) "ok"
array(2) {
  ["exposed"]=>
  bool(true)
  ["internal"]=>
  bool(true)
}
bool(false)
bool(false)
NULL
bool(false)
NULL
bool(true)
