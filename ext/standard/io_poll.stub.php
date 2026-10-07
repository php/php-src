<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace Io {
    class IoException extends \Exception {}
}

namespace Io\Poll {

    // Keep in sync with main/php_poll.h!
    enum Backend
    {
        case Auto;
        case Poll;
        case Epoll;
        case Kqueue;
        case EventPorts;
        case WSAPoll;

        /** @return list<Backend> */
        static public function getAvailableBackends(): array {}

        public function isAvailable(): bool {}

        public function supportsEdgeTriggering(): bool {}

        /** Whether Event::Priority is reported; kqueue and WSAPoll cannot. */
        public function supportsPriority(): bool {}

        /** Whether a ProcessHandle can be added: a pidfd on Linux, EVFILT_PROC on kqueue. */
        public function supportsProcessHandles(): bool {}

        /** Whether a SignalHandle can be added: a signalfd on Linux, EVFILT_SIGNAL on kqueue. */
        public function supportsSignalHandles(): bool {}
    }

    // Keep in sync with main/php_poll.h!
    enum Event {
        case Read;
        case Write;
        case Error;
        case HangUp;
        case ReadHangUp;
        case OneShot;
        case EdgeTriggered;
        /** Priority data (POLLPRI), backend dependent */
        case Priority;
        /** A TimerHandle fired */
        case Timer;
        /** A NotifyHandle is raised and not yet cleared */
        case Notify;
        /** A SignalHandle delivered a signal */
        case Signal;
        /** A ProcessHandle: the child exited and its status is in the handle */
        case Process;
    }

    interface Handle
    {
    }

    /**
     * A handle that holds its resource weakly: the resource can go away while
     * the handle is referenced, and the handle then reports invalid.
     */
    interface WeakHandle extends Handle
    {
    }

    /**
     * A deadline in the context, one-shot or periodic, on the monotonic clock
     * and counted from add(); a periodic one re-arms from its previous
     * deadline. Watched with Event::Timer; a fired one-shot timer is re-armed
     * by modifyEvents(), counted from then.
     * @strict-properties
     * @not-serializable
     */
    final class TimerHandle implements Handle
    {
        public function __construct(\Time\Duration $timeout, bool $periodic = false) {}

        public function getTimeout(): \Time\Duration {}

        public function isPeriodic(): bool {}
    }

    /**
     * Signals as events. The signals are blocked in the process signal mask
     * for the life of the handle, so they queue instead of running a
     * handler; a delivery is consumed and recorded when the context reports
     * Event::Signal.
     * @strict-properties
     * @not-serializable
     */
    final class SignalHandle implements Handle
    {
        /** @param list<int> $signals */
        public function __construct(array $signals) {}

        /** @return list<int> */
        public function getSignals(): array {}

        /** @return list<int> signals delivered since the previous call, in order */
        public function getDelivered(): array {}
    }

    /**
     * A process by pid. Each context reports Event::Process once when the
     * process exited, and getStatus() has its wait status then. The handle
     * never collects the child: proc_close() or pcntl_waitpid() still does
     * and returns at once. The status stays null for a process that is not
     * our child or was collected elsewhere.
     * @strict-properties
     * @not-serializable
     */
    final class ProcessHandle implements Handle
    {
        public function __construct(int $pid) {}

        /** @param resource $process a proc_open() resource */
        public static function fromProcess($process): static {}

        public function getPid(): int {}

        /** The wait status once the child exited, null before. */
        public function getStatus(): ?int {}
    }

    /**
     * A readiness source the program raises itself. notify() makes it ready
     * and it stays ready until clear() consumed every pending notification.
     * @strict-properties
     * @not-serializable
     */
    final class NotifyHandle implements Handle
    {
        public function __construct() {}

        public function notify(): void {}

        public function clear(): void {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Watcher
    {
        private final function __construct() {}

        public function getHandle(): Handle {}

        /** @return list<Event> */
        public function getWatchedEvents(): array {}

        /** @return list<Event> */
        public function getTriggeredEvents(): array {}

        public function getData(): mixed {}

        public function hasTriggered(Event $event): bool {}

        public function isActive(): bool {}

        public function modify(array $events, mixed $data = null): void {}

        public function modifyEvents(array $events): void {}

        public function modifyData(mixed $data): void {}

        public function remove(): void {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Context
    {
        public function __construct(Backend $backend = Backend::Auto) {}

        public function add(Handle $handle, array $events, mixed $data = null): Watcher {}

        /** @return list<Watcher> */
        public function wait(?\Time\Duration $timeout = null, ?int $maxEvents = null): array {}

        public function getBackend(): Backend {}

        /**
         * Called with each watcher the context lost without Watcher::remove(),
         * because its stream was closed or its handle invalidated. The calls
         * are made by the next wait(), before it polls.
         */
        public function onWatcherRemoved(?callable $callback = null): void {}
    }

    class PollException extends \Io\IoException {}

    abstract class FailedPollOperationException extends PollException
    {
        /** @cvalue PHP_POLL_ERROR_CODE_NONE */
        public const int ERROR_NONE = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_SYSTEM */
        public const int ERROR_SYSTEM = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_NOMEM */
        public const int ERROR_NOMEM = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_INVALID */
        public const int ERROR_INVALID = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_EXISTS */
        public const int ERROR_EXISTS = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_NOTFOUND */
        public const int ERROR_NOTFOUND = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_TIMEOUT */
        public const int ERROR_TIMEOUT = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_INTERRUPTED */
        public const int ERROR_INTERRUPTED = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_PERMISSION */
        public const int ERROR_PERMISSION = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_TOOBIG */
        public const int ERROR_TOOBIG = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_AGAIN */
        public const int ERROR_AGAIN = UNKNOWN;

        /** @cvalue PHP_POLL_ERROR_CODE_NOSUPPORT */
        public const int ERROR_NOSUPPORT = UNKNOWN;
    }

    class FailedContextInitializationException extends FailedPollOperationException {}

    class FailedHandleAddException extends FailedPollOperationException {}

    class FailedWatcherModificationException extends FailedPollOperationException {}

    class FailedPollWaitException extends FailedPollOperationException {}

    class BackendUnavailableException extends PollException {}

    class InactiveWatcherException extends PollException {}

    class HandleAlreadyWatchedException extends PollException {}

    class InvalidHandleException extends PollException {}
}

namespace {
    /**
     * One per stream, created by the core for its operations and registrations
     * or by create(); the stream keeps it while it is open.
     * @strict-properties
     * @not-serializable
     */
    final class StreamPollWeakHandle implements Io\Poll\WeakHandle
    {
        private function __construct() {}

        /** @param resource $stream */
        public static function create($stream): static {}

        /**
         * The stream, or null: closed, not exposed, or in an operation.
         * @return resource|null
         */
        public function getStream(): mixed {}

        /**
         * Whether getStream() may hand the stream out: created through create(),
         * or by the core for a stream a script holds
         */
        public function isExposed(): bool {}

        public function isValid(): bool {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class StreamPollHandle implements Io\Poll\Handle
    {
        /** @param resource $stream */
        public function __construct($stream) {}

        /** @return resource */
        public function getStream() {}

        public function isValid(): bool {}
    }
}
