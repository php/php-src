--TEST--
IO hooks: concurrent stream access from run() is rejected, the read arrives as a Recv
--FILE--
<?php

$server = stream_socket_server('tcp://127.0.0.1:0');
$addr = stream_socket_get_name($server, false);
$client = stream_socket_client("tcp://$addr");

class ConcurrentHook implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion {
        try {
            fgets($op->getHandle()->getStream());
        } catch (Error $e) {
            echo $e->getMessage() . "\n";
        }
        var_dump($op instanceof Io\Operation\Recv, $op->getEvents(), $op->getTimeout());
        return $op->complete(Io\CompletionStatus::Timeout);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

Io\Hooks\set_hooks(new ConcurrentHook());
stream_set_timeout($client, 3);
var_dump(fgets($client));
var_dump(stream_get_meta_data($client)['timed_out']);
?>
--EXPECTF--
Concurrent access to a stream
bool(true)
array(1) {
  [0]=>
  enum(Io\Poll\Event::Read)
}
object(Time\Duration)#%d (%d) {
  ["seconds"]=>
  int(%d)
  ["nanoseconds"]=>
  int(%d)
%A}
bool(false)
bool(true)
