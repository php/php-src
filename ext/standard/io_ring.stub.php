<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace Io\Ring {

    /** Which backend ior chose. Informational: there is no way to request one. */
    enum Backend {
        case IoUring;
        case Iocp;
        case Threads;
    }

    /**
     * The Ring: an operation queue that executes every operation itself,
     * on io_uring, IOCP or ior's thread pool.
     * @strict-properties
     * @not-serializable
     */
    final class Engine implements \Io\OperationQueue
    {
        /** Submission queue depth; 0 is the depth the core's own ring queue uses. */
        public function __construct(int $entries = 0) {}

        public function getBackend(): Backend {}

        /**
         * Raised for every posted completion, so a loop that keeps its own
         * Poll context can embed a ring: add it with Event::Notify and, when it
         * fires, call waitCompletions() with a zero timeout until it returns an
         * empty array.
         */
        public function getHandle(): \Io\Poll\NotifyHandle {}

        public function submit(\Io\Operation $op, mixed $data = null): void {}

        public function cancel(\Io\Operation $op): void {}

        public function add(\Io\Registration $registration): void {}

        public function remove(\Io\Registration $registration): void {}

        /**
         * @return list<\Io\Completion>
         */
        public function waitCompletions(?\Time\Duration $timeout = null, ?int $max = null): array {}

        public function countPending(): int {}

        /**
         * Capabilities worth reporting by default: none; see getSupportedHookCapabilities().
         * @return list<\Io\Hooks\Capability>
         */
        public function getHookCapabilities(): array {}

        /**
         * Capabilities this backend can serve, for a provider that opts in.
         * @return list<\Io\Hooks\Capability>
         */
        public function getSupportedHookCapabilities(): array {}
    }

    class RingException extends \Io\IoException {}

    /** Submit, cancel and wait failures, the errno in the code */
    class FailedRingOperationException extends RingException {}
}
