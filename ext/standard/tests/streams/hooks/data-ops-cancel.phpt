--TEST--
IO hooks: a provider that throws from run() cancels the operation, and a fiber destroyed while suspended in a read leaves the stream usable
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

// 1. Throwing from run() surfaces from the blocking function
Io\Hooks\set_hooks(new class implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return []; }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        throw new RuntimeException("cancelled " . $op::class);
    }
});
try {
    var_dump(fread($r, 10));
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
// The stream is not frozen afterwards
fwrite($w, "x");
Io\Hooks\set_hooks(null);
var_dump(fread($r, 10));

// 2. A fiber destroyed while suspended in fread(): the operation is orphaned.
// This provider keeps the fibers itself, so dropping one really destroys it.
final class Dropping implements Io\Hooks\Hooks
{
    private array $fibers = [];
    private array $ready = [];
    private int $inFlight = 0;
    public ?int $victim = null;

    public function __construct(private Io\OperationQueue $queue) {}
    public function getCapabilities(): array { return Scheduler::capabilities($this->queue); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}

    public function run(Io\Operation $op): Io\Completion
    {
        $fiber = Fiber::getCurrent();
        $id = spl_object_id($fiber);
        $this->fibers[$id] = $fiber;
        if ($op instanceof Io\Operation\Recv && $this->victim === null) {
            $this->victim = $id;
        }
        $this->queue->submit($op, $id);
        $this->inFlight++;
        unset($fiber);
        return Fiber::suspend();
    }

    public function drop(): void
    {
        unset($this->fibers[$this->victim]);
    }

    public function spawn(callable $fn): void { $this->ready[] = [new Fiber($fn), null]; }

    public function loop(): void
    {
        // countPending() rather than a counter of its own: an operation whose
        // fiber was destroyed is cancelled without a completion for anyone
        while ($this->ready || $this->queue->countPending() > 0) {
            while ($this->ready) {
                [$fiber, $value] = array_shift($this->ready);
                $fiber->isStarted() ? $fiber->resume($value) : $fiber->start();
                unset($fiber);
            }
            if ($this->queue->countPending() === 0) break;
            foreach ($this->queue->waitCompletions() as $c) {
                $this->inFlight--;
                $id = $c->getData();
                if (isset($this->fibers[$id])) {
                    $this->ready[] = [$this->fibers[$id], $c];
                    unset($this->fibers[$id]);
                }
            }
        }
    }
}

$scheduler = new Dropping(Scheduler::defaultQueue());
Io\Hooks\set_hooks($scheduler);
$scheduler->spawn(function () use ($r) {
    fread($r, 10);
    echo "read returned\n";
});
$scheduler->spawn(function () use ($scheduler, $r, $w) {
    usleep(5000);
    // Drop the only reference to the suspended fiber: its destructor unwinds it
    $scheduler->drop();
    // A freeze the queue kept for the orphan is lifted once the operation settled
    $tries = 0;
    while (true) {
        try {
            stream_get_meta_data($r);
            break;
        } catch (Error $e) {
            if (++$tries > 200) { echo "still frozen\n"; break; }
            usleep(1000);
        }
    }
    fwrite($w, "after");
    echo "writer done\n";
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
stream_set_blocking($r, false);
var_dump(fread($r, 10));
fclose($r);
fclose($w);
echo "done\n";
?>
--EXPECT--
cancelled Io\Operation\Recv
string(1) "x"
writer done
string(5) "after"
done
