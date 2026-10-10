--TEST--
IO hooks: a direct provider answering Ready to a Connect it did not perform gets a connected socket
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip unix sockets');
?>
--FILE--
<?php
final class ReadyOnly implements Io\Hooks\Hooks
{
    public array $ops = [];
    public function getCapabilities(): array { return [Io\Hooks\Capability::DirectData]; }
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ops[] = $op::class;
        return $op instanceof Io\Operation\Connect
            ? $op->completeReady($op->getEvents())
            : $op->complete(Io\CompletionStatus::Unsupported);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

$path = sys_get_temp_dir() . '/connect-direct-ready-' . getmypid() . '.sock';
@unlink($path);
$server = stream_socket_server("unix://$path");
$provider = new ReadyOnly();
Io\Hooks\set_hooks($provider);
$client = stream_socket_client("unix://$path", $errno, $errstr, 5);
Io\Hooks\set_hooks(null);
var_dump(is_resource($client), $provider->ops);
$peer = stream_socket_accept($server, 5);
var_dump(fwrite($client, "ping"), fread($peer, 4));
@unlink($path);
?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(20) "Io\Operation\Connect"
}
int(4)
string(4) "ping"
