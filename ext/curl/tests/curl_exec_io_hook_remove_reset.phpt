--TEST--
curl_exec() under io_hooks: a remove() that runs while libcurl closes a socket cannot reset the handle
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv('IO_HOOKS_QUEUE') === 'ring') {
    die("skip the Ring takes no Level registrations, so curl's sockets have none");
}
?>
--FILE--
<?php

include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

/* Resets the transfer's handle when the read registration of a curl socket is removed: only libcurl's close removes it */
class ResettingScheduler extends Scheduler
{
    public ?CurlHandle $ch = null;

    public function remove(\Io\Registration $registration): void
    {
        parent::remove($registration);
        if ($this->ch !== null && $registration->getHandle() instanceof CurlSocketPollWeakHandle
                && $registration->getEvent() === Io\Poll\Event::Read) {
            $ch = $this->ch;
            $this->ch = null;
            try {
                curl_reset($ch);
            } catch (Error $e) {
                echo $e->getMessage(), "\n";
            }
        }
    }
}

$scheduler = new ResettingScheduler();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    $request = '';
    while (!str_ends_with($request, "\r\n\r\n")) {
        $chunk = fread($conn, 1024);
        if ($chunk === false || $chunk === '') break;
        $request .= $chunk;
    }
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 2\r\n\r\nhi");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr, $scheduler) {
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_TIMEOUT, 5);
    $scheduler->ch = $ch;
    var_dump(curl_exec($ch));
});

$scheduler->loop();
?>
--EXPECT--
curl_reset(): Attempt to reset cURL handle from a callback
string(2) "hi"
