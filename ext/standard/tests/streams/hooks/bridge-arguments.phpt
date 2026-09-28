--TEST--
IO hooks: argument checks of the bridge methods
--FILE--
<?php
try {
    new Io\Poll\OperationQueue(new stdClass());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

final class Checks implements Io\Hooks\Hooks
{
    public Io\Poll\OperationQueue $queue;
    public function __construct() { $this->queue = new Io\Poll\OperationQueue(); }
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion
    {
        if ($op instanceof Io\Operation\GetAddrInfo) {
            try {
                $op->completeWithAddresses(["127.0.0.1\0garbage"]);
            } catch (ValueError $e) {
                echo $e->getMessage(), "\n";
            }
            return $op->completeWithAddresses(['127.0.0.1']);
        }
        if ($op instanceof Io\Operation\GetNameInfo) {
            try {
                $op->completeWithName("host\0garbage");
            } catch (ValueError $e) {
                echo $e->getMessage(), "\n";
            }
            return $op->completeWithName('localhost');
        }
        if ($op instanceof Io\Operation\Any) {
            $member = $op->getOperations()[0];
            try {
                $member->completeReady([]);
            } catch (ValueError $e) {
                echo $e->getMessage(), "\n";
            }
            $this->queue->submit($op);
            try {
                $this->queue->cancel($member);
            } catch (Io\IoException $e) {
                echo get_class($e), "\n";
            }
            $this->queue->cancel($op);
            $c = $member->completeReady([Io\Poll\Event::Read]);
            try {
                $op->completeWith([$c, $c]);
            } catch (ValueError $e) {
                echo $e->getMessage(), "\n";
            }
            return $op->completeWith([$c]);
        }
        return $op->complete(Io\CompletionStatus::Unsupported);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

$checks = new Checks();
Io\Hooks\set_hooks($checks);
var_dump(gethostbyname('bridge-arguments.invalid'));
var_dump(gethostbyaddr('192.0.2.1'));
[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$r = [$a];
$w = $e = null;
var_dump(stream_select($r, $w, $e, 5));
Io\Hooks\set_hooks(null);
?>
--EXPECT--
Io\Poll\OperationQueue::__construct(): Argument #1 ($context) must be of type ?Io\Poll\Context, stdClass given
Io\Operation\GetAddrInfo::completeWithAddresses(): Argument #1 ($addresses) must not contain any null bytes
string(9) "127.0.0.1"
Io\Operation\GetNameInfo::completeWithName(): Argument #1 ($host) must not contain any null bytes
string(9) "localhost"
Io\Operation::completeReady(): Argument #1 ($events) must not be empty
Io\IoException
Io\Operation\Any::completeWith(): Argument #1 ($completions) must not contain two completions of the same member
int(1)
