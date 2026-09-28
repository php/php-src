--TEST--
IO hooks: curl's socket directions and the streams' pairs are registrations bracketed by add() and remove()
--EXTENSIONS--
curl
--FILE--
<?php

include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $added = [];
    public array $curl = [];
    public array $removed = [];
    public array $runs = [];
    public bool $membersOk = true;
    public bool $addsOk = true;
    public bool $removesOk = true;

    private function seen(\Io\Operation $op): void
    {
        $reg = $op->getRegistration();
        $id = $reg === null ? -1 : spl_object_id($reg);
        $this->runs[$id] = ($this->runs[$id] ?? 0) + 1;
        $this->membersOk = $this->membersOk && $reg !== null && $reg->isValid()
            && isset($this->added[$id]) && $this->added[$id] === $reg
            && $op->getEvents() === [$reg->getEvent()]
            && $op->getHandle() === $reg->getHandle();
    }

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Any) {
            // curl: one Poll member per registered direction
            foreach ($op->getOperations() as $m) {
                if ($m instanceof \Io\Operation\Poll) {
                    $this->seen($m);
                }
            }
        } elseif ($op instanceof \Io\Operation\Accept || $op instanceof \Io\Operation\Recv) {
            // the server's streams: the wait after EAGAIN is on the stream's pair
            $this->seen($op);
        }
        return parent::run($op);
    }

    public function add(\Io\Registration $registration): void
    {
        $this->added[spl_object_id($registration)] = $registration;
        $handle = $registration->getHandle();
        if ($handle instanceof \Io\Curl\SocketWeakHandle) {
            $this->curl[spl_object_id($registration)] = true;
        }
        // The stream pairs want Edge and get Level, all this provider takes
        $this->addsOk = $this->addsOk && $registration->isValid()
            && $registration->getTrigger() === \Io\Poll\Trigger::Level
            && ($handle instanceof \Io\Curl\SocketWeakHandle || $handle instanceof \StreamPollWeakHandle)
            && in_array($registration->getEvent(), [\Io\Poll\Event::Read, \Io\Poll\Event::Write], true);
        parent::add($registration);
    }

    public function remove(\Io\Registration $registration): void
    {
        $this->removed[spl_object_id($registration)] = true;
        $this->removesOk = $this->removesOk && isset($this->added[spl_object_id($registration)])
            && $this->added[spl_object_id($registration)] === $registration && $registration->isValid();
        parent::remove($registration);
    }
}

// The Poll queue takes Level registrations, the ring does not
$scheduler = new Tracing(new Io\Poll\OperationQueue(), [Io\Hooks\Capability::LevelRegistrations]);
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
    fwrite($conn, "HTTP/1.0 200 OK\r\nContent-Length: 2\r\n\r\nok");
    fclose($conn);
});

$scheduler->spawn(function () use ($addr) {
    $ch = curl_init("http://$addr/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    var_dump(curl_exec($ch));
});

$scheduler->loop();
fclose($server);

// Every pair was added once, waited on with the same object, and removed
var_dump(count($scheduler->curl) >= 1, count($scheduler->added) > count($scheduler->curl));
$removed = array_keys($scheduler->removed);
$added = array_keys($scheduler->added);
sort($removed);
sort($added);
var_dump($removed === $added);
var_dump($scheduler->addsOk, $scheduler->removesOk, $scheduler->membersOk);
$ran = true;
$invalid = true;
foreach ($scheduler->added as $id => $reg) {
    $ran = $ran && ($scheduler->runs[$id] ?? 0) >= 1;
    $invalid = $invalid && !$reg->isValid();
}
var_dump($ran, $invalid);
try {
    reset($scheduler->added)->getEvent();
} catch (Io\InvalidRegistrationException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);
?>
--EXPECT--
string(2) "ok"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
The registration has ended
