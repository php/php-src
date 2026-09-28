--TEST--
IO hooks: Any members a provider submitted on their own are withdrawn when the Any ends
--FILE--
<?php
final class MemberProvider implements Io\Hooks\Hooks
{
    public Io\Poll\OperationQueue $queue;

    public function __construct() { $this->queue = new Io\Poll\OperationQueue(); }
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion
    {
        if ($op instanceof Io\Operation\Any) {
            foreach ($op->getOperations() as $member) {
                $this->queue->submit($member);
            }
            return $op->completeWith($this->queue->waitCompletions());
        }
        $this->queue->submit($op);
        return $this->queue->waitCompletions()[0];
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

$family = PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX;
[$a1, $a2] = stream_socket_pair($family, STREAM_SOCK_STREAM, 0);
[$b1, $b2] = stream_socket_pair($family, STREAM_SOCK_STREAM, 0);
$provider = new MemberProvider();
Io\Hooks\set_hooks($provider);

fwrite($a2, "x");
$r = [$a1, $b1];
$w = $e = null;
var_dump(stream_select($r, $w, $e, 5), $r === [$a1]);
var_dump($provider->queue->countPending());

fwrite($b2, "y");
var_dump(count($provider->queue->waitCompletions(Time\Duration::fromNanoseconds(0))));
Io\Hooks\set_hooks(null);
?>
--EXPECT--
int(1)
bool(true)
int(0)
int(0)
