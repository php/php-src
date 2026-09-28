<?php

/* The minimal provider: a fiber scheduler that is a complete provider on any
 * Io\OperationQueue. The Hooks methods are called by the core from inside
 * blocking functions, on the fiber that runs them; spawn() and loop() are
 * what the script calls. */
class Scheduler implements \Io\Hooks\Hooks
{
    /** Fibers to resume, with the value: null to start, a Completion to continue. */
    private array $ready = [];
    /** Submitted operations whose fiber is suspended in run(). */
    private int $inFlight = 0;
    private \Io\OperationQueue $queue;
    private array $capabilities;

    public function __construct(?\Io\OperationQueue $queue = null, ?array $capabilities = null)
    {
        $this->queue = $queue ?? self::defaultQueue();
        $this->capabilities = self::capabilities($this->queue, $capabilities);
    }

    /* IO_HOOKS_QUEUE=ring runs the same tests on Io\Ring\Engine */
    public static function defaultQueue(): \Io\OperationQueue
    {
        if (getenv('IO_HOOKS_QUEUE') === 'ring') {
            if (!class_exists(\Io\Ring\Engine::class)) {
                die("skip Io\\Ring\\Engine not available\n");
            }
            return new \Io\Ring\Engine();
        }
        return new \Io\Poll\OperationQueue();
    }

    /* IO_HOOKS_CAPS=files,direct opts in to what the queue supports, default is its own set */
    public static function capabilities(\Io\OperationQueue $queue, ?array $wanted = null): array
    {
        if ($wanted === null) {
            $env = (string) getenv('IO_HOOKS_CAPS');
            if ($env === '') {
                return $queue->getHookCapabilities();
            }
            $wanted = array_map(
                fn ($name) => constant(\Io\Hooks\Capability::class . '::' . ucfirst(trim($name))),
                explode(',', $env));
        }
        $supported = $queue instanceof \Io\Ring\Engine
            ? $queue->getSupportedHookCapabilities()
            : $queue->getHookCapabilities();
        return array_values(array_filter($supported, fn ($c) => in_array($c, $wanted, true)));
    }

    /** Schedule a task; it starts on the next pass of loop(). */
    public function spawn(callable $fn): void
    {
        $this->ready[] = [new \Fiber($fn), null];
    }

    /* The Hooks interface, called by the core. */

    public function getCapabilities(): array
    {
        return $this->capabilities;
    }

    public function run(\Io\Operation $op): \Io\Completion
    {
        $this->queue->submit($op, \Fiber::getCurrent());
        $this->inFlight++;
        return \Fiber::suspend();
    }

    public function add(\Io\Registration $registration): void
    {
        $this->queue->add($registration);
    }

    public function remove(\Io\Registration $registration): void
    {
        $this->queue->remove($registration);
    }

    /* The event loop, called by the script on the main fiber. */
    public function loop(): void
    {
        while ($this->ready || $this->inFlight > 0) {
            while ($this->ready) {
                [$fiber, $value] = array_shift($this->ready);
                $fiber->isStarted() ? $fiber->resume($value) : $fiber->start();
            }
            if ($this->inFlight === 0) {
                break;
            }
            foreach ($this->queue->waitCompletions() as $c) {
                $this->inFlight--;
                $this->ready[] = [$c->getData(), $c];
            }
        }
    }

    /* Test helper: start a task and let it run before continuing the caller. */
    public function go(callable $fn): void
    {
        $this->ready[] = [new \Fiber($fn), null];
        $fiber = \Fiber::getCurrent();
        if ($fiber === null) {
            return;
        }
        $this->ready[] = [$fiber, null];
        \Fiber::suspend();
    }
}
