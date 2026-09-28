<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace Io {

    enum CompletionStatus {
        case Done;
        case Ready;
        case Timeout;
        case Interrupted;
        case Cancelled;
        case Unsupported;
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    abstract class Operation
    {
        private function __construct() {}

        /**
         * A WeakHandle for descriptor based operations and Fsync, a
         * TimerHandle for Timer, a ProcessHandle for WaitPid on one child,
         * a SignalHandle for SigWait; null for DNS, Any and WaitPid on any
         * child. The generic handles are created on the first call.
         */
        public function getHandle(): ?Poll\Handle {}

        /** The registration of the pair a wait is on; null when the pair is not registered. */
        public function getRegistration(): ?Registration {}

        /** @return list<Poll\Event> the events that let the op proceed; empty when there is no handle */
        public function getEvents(): array {}

        /** The remaining time, computed at call time; null when there is no deadline. */
        public function getTimeout(): ?\Time\Duration {}

        /** False once the operation ended. */
        public function isValid(): bool {}

        /** For providers that complete an operation themselves. */
        public function complete(CompletionStatus $status, int $result = 0, int $error = 0): Completion {}

        /**
         * Readiness observed for the operation's handle: Done for a Poll or
         * Timer operation, Ready for every other type.
         * @param list<Poll\Event> $events
         */
        public function completeReady(array $events): Completion {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Completion
    {
        private function __construct() {}

        public function getOperation(): Operation {}

        public function getStatus(): CompletionStatus {}

        /** Bytes, a descriptor, or a Poll event mask as int. */
        public function getResult(): int {}

        /** @return list<Poll\Event> for Poll operations and Ready completions */
        public function getEvents(): array {}

        public function getError(): int {}

        /** The $data passed to OperationQueue::submit(). */
        public function getData(): mixed {}

        /** @return list<Completion> members of an Any that had completed; empty otherwise */
        public function getCompletions(): array {}
    }

    class InvalidOperationException extends IoException {}

    /**
     * A registered (handle, event) pair: waits on it repeat until it is removed. The core
     * creates it and hands the same object to Hooks::add(), Hooks::remove() and every wait's
     * Operation::getRegistration(). Valid from add() until remove() returned or the provider
     * was replaced.
     * @strict-properties
     * @not-serializable
     */
    final class Registration
    {
        private function __construct() {}

        /** The pair's handle, created on the first call. */
        public function getHandle(): Poll\WeakHandle {}

        /** Read or Write */
        public function getEvent(): Poll\Event {}

        public function getTrigger(): Poll\Trigger {}

        /** False once the registration ended. */
        public function isValid(): bool {}
    }

    class InvalidRegistrationException extends IoException {}

    interface OperationQueue
    {
        public function submit(Operation $op, mixed $data = null): void;

        public function cancel(Operation $op): void;

        /** Waits on the pair will repeat until remove(); a trigger the queue cannot keep is served one-shot. */
        public function add(Registration $registration): void;

        /** Before the descriptor closes; no wait on the pair is in flight. */
        public function remove(Registration $registration): void;

        /** @return list<Completion> */
        public function waitCompletions(?\Time\Duration $timeout = null, ?int $max = null): array;

        /** Submitted and not yet completed. */
        public function countPending(): int;

        /** @return list<Hooks\Capability> */
        public function getHookCapabilities(): array;
    }
}

namespace Io\Operation {

    final class Poll extends \Io\Operation {}

    final class Timer extends \Io\Operation {}

    final class Read extends \Io\Operation
    {
        public function getLength(): int {}

        /** -1 for the current position */
        public function getOffset(): int {}
    }

    final class Write extends \Io\Operation
    {
        public function getLength(): int {}

        public function getOffset(): int {}
    }

    final class Recv extends \Io\Operation
    {
        public function getLength(): int {}

        /** MSG_* */
        public function getFlags(): int {}
    }

    final class Send extends \Io\Operation
    {
        public function getLength(): int {}

        public function getFlags(): int {}
    }

    final class Accept extends \Io\Operation {}

    final class Connect extends \Io\Operation
    {
        /** Textual, as stream_socket_get_name() */
        public function getAddress(): string {}
    }

    final class Fsync extends \Io\Operation
    {
        public function isDataOnly(): bool {}
    }

    /** A wait for a child; the handle is a Poll\ProcessHandle for a pid, null for any child. */
    final class WaitPid extends \Io\Operation
    {
        /** The pid, or -1 for any child */
        public function getPid(): int {}
    }

    /** A wait for one of a set of signals; the handle is a Poll\SignalHandle over the set. */
    final class SigWait extends \Io\Operation
    {
        /** @return list<int> */
        public function getSignals(): array {}
    }

    /** A name lookup; no handle. */
    final class GetAddrInfo extends \Io\Operation
    {
        public function getHost(): string {}

        public function getService(): ?string {}

        /**
         * Complete with resolved addresses from a userland resolver; the
         * core builds the address list. @param list<string> $addresses IPs
         */
        public function completeWithAddresses(array $addresses): \Io\Completion {}
    }

    /** A reverse lookup; no handle. */
    final class GetNameInfo extends \Io\Operation
    {
        /** Textual, as stream_socket_get_name() */
        public function getAddress(): string {}

        public function completeWithName(string $host, ?string $service = null): \Io\Completion {}
    }

    /** Wait on several Poll and Timer operations at once. */
    final class Any extends \Io\Operation
    {
        /** @return list<\Io\Operation> */
        public function getOperations(): array {}

        /**
         * For providers that complete members themselves. Each entry was
         * made from a member with completeReady() or complete().
         * @param list<\Io\Completion> $completions
         */
        public function completeWith(array $completions): \Io\Completion {}
    }
}

namespace Io\Poll {

    /** What the waits on a registered pair tolerate. */
    enum Trigger {
        /** Every readiness wait follows a drain: recorded readiness may answer it. */
        case Edge;
        /** Readiness must be current at arm time. */
        case Level;
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class OperationQueue implements \Io\OperationQueue
    {
        public function __construct(?Context $context = null) {}

        /** The loop may still add its own watchers to it. */
        public function getContext(): Context {}

        public function submit(\Io\Operation $op, mixed $data = null): void {}

        public function cancel(\Io\Operation $op): void {}

        public function add(\Io\Registration $registration): void {}

        public function remove(\Io\Registration $registration): void {}

        /** @return list<\Io\Completion> */
        public function waitCompletions(?\Time\Duration $timeout = null, ?int $max = null): array {}

        public function countPending(): int {}

        /** @return list<\Io\Hooks\Capability> */
        public function getHookCapabilities(): array {}
    }
}

namespace Io\Hooks {

    interface Hooks
    {
        /** @return list<Capability> */
        public function getCapabilities(): array;

        /**
         * Execute one operation the core handed over and return its
         * completion, or throw to cancel it.
         */
        public function run(\Io\Operation $op): \Io\Completion;

        /**
         * Waits on the pair will repeat until remove(). Called only with the capability for
         * the pair's trigger, at most once per pair.
         */
        public function add(\Io\Registration $registration): void;

        /** The pair is done, before its descriptor closes; no wait on it is in flight. */
        public function remove(\Io\Registration $registration): void;
    }

    enum Capability {
        /** Regular file ops and Fsync reach the provider, which performs them itself. */
        case Files;
        /** Read, Write, Recv, Send and Connect are submitted before the core's syscall-first attempt. */
        case DirectData;
        /** Accept is submitted before accept(). */
        case DirectAccept;
        /** add() and remove() with Trigger::Edge. */
        case EdgeRegistrations;
        /** add() and remove() with Trigger::Level. */
        case LevelRegistrations;
    }

    /** Installs the provider, returns the previous userland one. Throws if a C provider is active. */
    function set_hooks(?Hooks $hooks): ?Hooks {}

    /** The installed userland provider; null when none is installed or a C provider owns the request. */
    function get_hooks(): ?Hooks {}

    /** True when any provider is installed, C or userland. */
    function is_active(): bool {}
}
