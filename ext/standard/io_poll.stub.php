<?php

/** @generate-class-entries */

namespace Io {
    class IoException extends \Exception {}
}

namespace Io\Poll {

    enum Backend
    {
        case Auto;
        case Poll;
        case Epoll;
        case Kqueue;
        case EventPorts;
        case WSAPoll;

        static public function getAvailableBackends(): array {}

        public function isAvailable(): bool {}

        public function supportsEdgeTriggering(): bool {}
    }

    enum Event {
        case Read;
        case Write;
        case Error;
        case HangUp;
        case ReadHangUp;
        case OneShot;
        case EdgeTriggered;
    }

    abstract class Handle
    {
        protected function getFileDescriptor(): int {}
    }

    final class Watcher
    {
        private final function __construct() {}

        public function getHandle(): Handle {}

        public function getWatchedEvents(): array {}

        public function getTriggeredEvents(): array {}

        public function getData(): mixed {}

        public function hasTriggered(Event $event): bool {}

        public function isActive(): bool {}

        public function modify(array $events, mixed $data = null): void {}

        public function modifyEvents(array $events): void {}

        public function modifyData(mixed $data): void {}

        public function remove(): void {}
    }

    final class Context
    {
        public function __construct(Backend $backend = Backend::Auto) {}

        public function add(Handle $handle, array $events, mixed $data = null): Watcher {}

        public function wait(int $timeoutSeconds = -1, int $timeoutMicroseconds = 0, int $maxEvents = -1): array {}

        public function getBackend(): Backend {}
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

    class InactiveWatcherException extends PollException {}

    class HandleAlreadyWatchedException extends PollException {}

    class InvalidHandleException extends PollException {}
}

namespace {
    final class StreamPollHandle extends Io\Poll\Handle
    {
        /** @param resource $stream */
        public function __construct($stream) {}

        /** @return resource */
        public function getStream() {}

        public function isValid(): bool {}
    }
}
