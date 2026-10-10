--TEST--
IO hooks: name lookups reach the provider as GetAddrInfo and GetNameInfo, answered from userland or by the library
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Resolver extends Scheduler
{
    public array $seen = [];
    public ?string $answer = null;
    public ?string $name = null;
    public bool $unsupported = false;

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\GetAddrInfo) {
            $this->seen[] = 'GetAddrInfo ' . $op->getHost() . ' ' . var_export($op->getService(), true)
                . ' ' . var_export($op->getHandle(), true) . ' ' . var_export($op->getTimeout(), true);
            if ($this->unsupported) {
                return $op->complete(\Io\CompletionStatus::Unsupported);
            }
            return $op->completeWithAddresses($this->answer === null ? [] : [$this->answer]);
        }
        if ($op instanceof \Io\Operation\GetNameInfo) {
            $this->seen[] = 'GetNameInfo ' . $op->getAddress();
            if ($this->unsupported) {
                return $op->complete(\Io\CompletionStatus::Unsupported);
            }
            return $op->completeWithName($this->name);
        }
        return parent::run($op);
    }
}

$resolver = new Resolver();
Io\Hooks\set_hooks($resolver);

$server = stream_socket_server('tcp://127.0.0.1:0');
$port = (int) substr(strrchr(stream_socket_get_name($server, false), ':'), 1);

// 1. A userland answer: the name is whatever the provider says
$resolver->answer = '127.0.0.1';
$resolver->spawn(function () use ($server) {
    $conn = stream_socket_accept($server, 5);
    fwrite($conn, "hi\n");
    fclose($conn);
});
$resolver->spawn(function () use ($port) {
    $c = stream_socket_client("tcp://resolved.by.provider:$port", $errno, $errstr, 5);
    var_dump($c !== false, fgets($c));
});
$resolver->loop();

// 2. No address: the lookup fails
$resolver->answer = null;
$c = @stream_socket_client("tcp://nowhere.provider:$port", $errno, $errstr, 1);
var_dump($c, $errno);

// 3. Unsupported: the library resolves it
$resolver->unsupported = true;
var_dump(gethostbyname('localhost'));
var_dump(gethostbynamel('localhost') !== false);

// 4. Reverse lookups
$resolver->unsupported = false;
$resolver->name = 'named.by.provider';
var_dump(gethostbyaddr('127.0.0.1'));
$resolver->unsupported = true;
var_dump(is_string(gethostbyaddr('127.0.0.1')));

Io\Hooks\set_hooks(null);
foreach ($resolver->seen as $line) {
    echo $line, "\n";
}
?>
--EXPECTF--
bool(true)
string(3) "hi
"
bool(false)
int(%d)
string(9) "127.0.0.1"
bool(true)
string(17) "named.by.provider"
bool(true)
GetAddrInfo resolved.by.provider NULL NULL NULL
GetAddrInfo nowhere.provider NULL NULL NULL
GetAddrInfo localhost NULL NULL NULL
GetAddrInfo localhost NULL NULL NULL
GetNameInfo 127.0.0.1:0
GetNameInfo 127.0.0.1:0
