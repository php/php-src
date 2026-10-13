--TEST--
curl_exec() under io_hooks: an exception from remove() while libcurl closes a socket ends the transfer, and libcurl's callbacks still run meanwhile
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

/* Throws once, when the read registration of a curl socket is removed: only libcurl's close removes it */
class ThrowingScheduler extends Scheduler
{
    public bool $thrown = false;

    public function remove(\Io\Registration $registration): void
    {
        parent::remove($registration);
        if (!$this->thrown && $registration->getHandle() instanceof CurlSocketPollWeakHandle
                && $registration->getEvent() === Io\Poll\Event::Read) {
            $this->thrown = true;
            throw new RuntimeException("remove failed");
        }
    }
}

$scheduler = new ThrowingScheduler();
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
    fwrite($conn, "HTTP/1.0 302 Found\r\nLocation: /next\r\nContent-Length: 0\r\n\r\n");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr) {
    $ch = curl_init("http://$addr/");
    curl_setopt_array($ch, [
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_FOLLOWLOCATION => true,
        CURLOPT_TIMEOUT => 5,
        CURLOPT_VERBOSE => true,
        /* libcurl logs the follow after it closed the redirect's connection */
        CURLOPT_DEBUGFUNCTION => function ($ch, $type, $data) {
            if ($type === CURLINFO_TEXT && str_starts_with($data, 'Issue another request')) {
                echo "follow logged\n";
            }
            return 0;
        },
    ]);
    try {
        curl_exec($ch);
        echo "no exception\n";
    } catch (RuntimeException $e) {
        echo get_class($e), ": ", $e->getMessage(), "\n";
    }
    var_dump(curl_errno($ch) === CURLE_ABORTED_BY_CALLBACK);
});

$scheduler->loop();
?>
--EXPECT--
follow logged
RuntimeException: remove failed
bool(true)
