<?php

/** @generate-class-entries */

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

        public function wait(int $timeout = -1, int $maxEvents = -1): array {}

        public function getBackend(): Backend {}
    }

    class PollException extends Exception
    {
    }
}

namespace {
    final class StreamPollHandle extends PollHandle
    {
        /** @param resource $stream */
        public function __construct($stream) {}

        /** @return resource */
        public function getStream() {}

        public function isValid(): bool {}
    }
}
