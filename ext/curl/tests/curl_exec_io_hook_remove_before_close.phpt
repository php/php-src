--TEST--
curl_exec() under io_hooks: a socket libcurl closes leaves the provider before it is closed
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

/* Keeps the registrations of curl's sockets the provider holds */
class WatchingScheduler extends Scheduler
{
    /** @var array<int, Io\Registration> */
    public array $curlRegistrations = [];

    public function add(\Io\Registration $registration): void
    {
        if ($registration->getHandle() instanceof CurlSocketPollWeakHandle) {
            $this->curlRegistrations[spl_object_id($registration)] = $registration;
        }
        parent::add($registration);
    }

    public function remove(\Io\Registration $registration): void
    {
        unset($this->curlRegistrations[spl_object_id($registration)]);
        parent::remove($registration);
    }

    public function closedSocketRegistrations(): int
    {
        $count = 0;
        foreach ($this->curlRegistrations as $registration) {
            if (!$registration->getHandle()->isValid()) {
                $count++;
            }
        }
        return $count;
    }
}

$scheduler = new WatchingScheduler();
Io\Hooks\set_hooks($scheduler);

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);

$scheduler->spawn(function () use ($server) {
    foreach (["HTTP/1.0 302 Found\r\nLocation: /next\r\nContent-Length: 0\r\n\r\n", "HTTP/1.0 200 OK\r\nContent-Length: 2\r\n\r\nhi"] as $response) {
        $conn = stream_socket_accept($server, 5);
        $request = '';
        while (!str_ends_with($request, "\r\n\r\n")) {
            $chunk = fread($conn, 1024);
            if ($chunk === false || $chunk === '') break;
            $request .= $chunk;
        }
        fwrite($conn, $response);
        fclose($conn);
    }
});

$scheduler->spawn(function () use ($addr, $scheduler) {
    $ch = curl_init("http://$addr/");
    curl_setopt_array($ch, [
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_FOLLOWLOCATION => true,
        CURLOPT_TIMEOUT => 5,
        CURLOPT_VERBOSE => true,
        /* libcurl logs the follow after it closed the redirect's connection */
        CURLOPT_DEBUGFUNCTION => function ($ch, $type, $data) use ($scheduler) {
            if ($type === CURLINFO_TEXT && str_starts_with($data, 'Issue another request')) {
                echo "registrations of closed sockets at the follow: ", $scheduler->closedSocketRegistrations(), "\n";
            }
            return 0;
        },
    ]);
    var_dump(curl_exec($ch));
});

$scheduler->loop();
?>
--EXPECT--
registrations of closed sockets at the follow: 0
string(2) "hi"
