--TEST--
IO hooks: a provider completing Timer, Poll and Any operations itself
--EXTENSIONS--
curl
--FILE--
<?php

final class Recorder implements Io\Hooks\Hooks
{
    public array $seen = [];
    public int $anyRuns = 0;
    public bool $membersOk = true;
    public bool $timerOk = true;

    public function getCapabilities(): array { return []; }

    public function run(Io\Operation $op): Io\Completion
    {
        $this->seen[$op::class] = true;
        if ($op instanceof Io\Operation\Timer) {
            $this->timerOk = $this->timerOk
                && $op->getHandle() instanceof Io\Poll\TimerHandle
                && $op->getEvents() === [Io\Poll\Event::Timer]
                && $op->getTimeout() instanceof Time\Duration;
            return $op->complete(Io\CompletionStatus::Done);
        }
        if ($op instanceof Io\Operation\Any) {
            $this->anyRuns++;
            $members = $op->getOperations();
            $completions = [];
            foreach ($members as $m) {
                if ($m instanceof Io\Operation\Timer) {
                    $completions[] = $m->complete(Io\CompletionStatus::Done);
                } else {
                    /* Poll members carry a weak handle and no deadline of their own */
                    $this->membersOk = $this->membersOk
                        && $m instanceof Io\Operation\Poll
                        && $m->getHandle() instanceof Io\Poll\WeakHandle
                        && $m->getTimeout() === null
                        && $m->getEvents() !== [];
                    /* Report every member ready: libcurl copes with spurious readiness */
                    $completions[] = $m->completeReady($m->getEvents());
                }
            }
            $c = $op->completeWith($completions);
            $this->membersOk = $this->membersOk && count($c->getCompletions()) === count($members);
            return $c;
        }
        return $op->completeReady($op->getEvents());
    }

    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

$recorder = new Recorder();
Io\Hooks\set_hooks($recorder);

usleep(1);

$ch = curl_init("http://127.0.0.1:1/");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_CONNECTTIMEOUT_MS, 200);
var_dump(curl_exec($ch));
var_dump(curl_errno($ch) !== 0);

Io\Hooks\set_hooks(null);
var_dump(array_keys($recorder->seen), $recorder->anyRuns >= 1, $recorder->membersOk, $recorder->timerOk);

/* Operations end when run() returns */
$last = null;
Io\Hooks\set_hooks(new class($last) implements Io\Hooks\Hooks {
    public function __construct(private &$last) {}
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion {
        $this->last = $op;
        return $op->complete(Io\CompletionStatus::Done);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
});
usleep(1);
var_dump($last instanceof Io\Operation\Timer, $last->isValid());
try {
    $last->getTimeout();
} catch (Io\InvalidOperationException $e) {
    echo $e->getMessage(), "\n";
}
Io\Hooks\set_hooks(null);
?>
--EXPECT--
bool(false)
bool(true)
array(2) {
  [0]=>
  string(18) "Io\Operation\Timer"
  [1]=>
  string(16) "Io\Operation\Any"
}
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
The operation has ended
