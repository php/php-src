--TEST--
Io\Ring\Engine: backend, notification handle and a Timer operation
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php

$ring = new Io\Ring\Engine();
var_dump($ring instanceof Io\OperationQueue);
var_dump($ring->getBackend() instanceof Io\Ring\Backend);
var_dump($ring->countPending());

$handle = $ring->getHandle();
var_dump($handle instanceof Io\Poll\NotifyHandle);
try {
    $handle->notify();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// A Timer op through a provider that submits to the ring and reaps it
final class RingProvider implements Io\Hooks\Hooks
{
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getHookCapabilities(); }
    public function add(Io\Operation $op): void {}
    public function remove(Io\Operation $op): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op, "data");
        var_dump($this->ring->countPending());
        $completions = [];
        while (!$completions) {
            $completions = $this->ring->waitCompletions(Time\Duration::fromSeconds(2));
        }
        var_dump(count($completions), $completions[0]->getOperation() === $op, $completions[0]->getData());
        var_dump($completions[0]->getStatus(), $this->ring->countPending());
        return $completions[0];
    }
}

Io\Hooks\set_hooks(new RingProvider($ring));
$start = hrtime(true);
usleep(20000);
$elapsed_ms = (hrtime(true) - $start) / 1e6;
var_dump($elapsed_ms >= 15 && $elapsed_ms < 1000);
Io\Hooks\set_hooks(null);

// The notification descriptor reports a completion posted while nobody waited
$ctx = new Io\Poll\Context();
$watcher = $ctx->add($handle, [Io\Poll\Event::Notify]);
Io\Hooks\set_hooks(new class($ring) implements Io\Hooks\Hooks {
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return []; }
    public function add(Io\Operation $op): void {}
    public function remove(Io\Operation $op): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        global $ctx, $handle;
        $this->ring->submit($op);
        // Clear, then reap until empty; a wakeup that finds nothing is a stale
        // notification from an earlier blocking wait and is harmless
        $completions = [];
        $wakeups = 0;
        while (!$completions) {
            $events = $ctx->wait(Time\Duration::fromSeconds(2));
            $wakeups++;
            if (count($events) !== 1 || !$events[0]->hasTriggered(Io\Poll\Event::Notify)) {
                echo "unexpected events\n";
                break;
            }
            $completions = $this->ring->waitCompletions(Time\Duration::fromSeconds(0));
        }
        var_dump($wakeups >= 1, count($completions));
        var_dump($this->ring->waitCompletions(Time\Duration::fromSeconds(0)));
        return $completions[0];
    }
});
usleep(1000);
Io\Hooks\set_hooks(null);
$watcher->remove();
echo "done\n";
?>
--EXPECT--
bool(true)
bool(true)
int(0)
bool(true)
This Io\Poll\NotifyHandle is raised by its owner and cannot be notified
int(1)
int(1)
bool(true)
string(4) "data"
enum(Io\CompletionStatus::Done)
int(0)
bool(true)
bool(true)
int(1)
array(0) {
}
done
