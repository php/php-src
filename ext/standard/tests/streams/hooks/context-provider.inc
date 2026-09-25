<?php

/* A provider on one Poll context, every operation awaited
 * through its handle, a timer pair for the deadline. */
class ContextProvider implements \Io\Hooks\Hooks
{
    private \Io\Poll\Context $context;
    private array $ready = [];
    private int $waiting = 0;

    public function __construct()
    {
        $this->context = new \Io\Poll\Context();
    }

    public function getCapabilities(): array { return []; }
    public function add(\Io\Operation $op): void {}
    public function remove(\Io\Operation $op): void {}

    public function spawn(callable $fn): void
    {
        $this->ready[] = [new \Fiber($fn), null];
    }

    private function await(array $pairs): array
    {
        $fiber = \Fiber::getCurrent();
        $watchers = [];
        foreach ($pairs as $i => [$handle, $events]) {
            $watchers[$i] = $this->context->add($handle, [...$events, \Io\Poll\Event::OneShot], [$fiber, $i]);
        }
        $this->waiting++;
        [$i, $triggered] = \Fiber::suspend();
        // A fired one-shot watcher stays registered and disarmed, so every
        // watcher of this await goes, or the next await on the handle is refused
        foreach ($watchers as $w) {
            if ($w->isActive()) {
                $w->remove();
            }
        }
        return [$i, $triggered];
    }

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Any) {
            $members = $op->getOperations();
            $pairs = array_map(fn ($m) => [$m->getHandle(), $m->getEvents()], $members);
            [$i, $triggered] = $this->await($pairs);
            return $op->completeWith([$members[$i]->completeReady($triggered)]);
        }
        if ($op->getHandle() === null) {
            return $op->complete(\Io\CompletionStatus::Unsupported);
        }
        $pairs = [[$op->getHandle(), $op->getEvents()]];
        if (!$op instanceof \Io\Operation\Timer && $op->getTimeout() !== null) {
            $pairs[] = [new \Io\Poll\TimerHandle($op->getTimeout()), [\Io\Poll\Event::Timer]];
        }
        [$i, $triggered] = $this->await($pairs);
        return $i === 0
            ? $op->completeReady($triggered)
            : $op->complete(\Io\CompletionStatus::Timeout);
    }

    public function loop(): void
    {
        while ($this->ready || $this->waiting > 0) {
            while ($this->ready) {
                [$fiber, $value] = array_shift($this->ready);
                $fiber->isStarted() ? $fiber->resume($value) : $fiber->start();
            }
            if ($this->waiting === 0) {
                break;
            }
            $resumed = [];
            foreach ($this->context->wait() as $w) {
                [$fiber, $i] = $w->getData();
                if (isset($resumed[spl_object_id($fiber)])) {
                    continue;
                }
                $resumed[spl_object_id($fiber)] = true;
                $this->waiting--;
                $this->ready[] = [$fiber, [$i, $w->getTriggeredEvents()]];
            }
        }
    }
}

