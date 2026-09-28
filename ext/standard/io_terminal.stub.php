<?php

/**
 * @generate-class-entries
 * @generate-c-enums
 */

namespace Io\Terminal {

    /** @strict-properties */
    class TerminalException extends \Io\IoException {}

    enum Key
    {
        case Up;
        case Down;
        case Right;
        case Left;
        case Enter;
        case Backspace;
        case Escape;
        case Tab;
        case Home;
        case End;
        case Delete;
        case PageUp;
        case PageDown;
        case Resize;
        case F1;
        case F2;
        case F3;
        case F4;
        case F5;
        case F6;
        case F7;
        case F8;
        case F9;
        case F10;
        case F11;
        case F12;
    }

    /** @strict-properties */
    final readonly class TerminalSize
    {
        public readonly int $cols;
        public readonly int $rows;

        public function __construct(int $cols, int $rows) {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class ModeToken
    {
        private function __construct() {}
    }

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Terminal
    {
        private function __construct() {}

        public static function fromStdio(): Terminal {}

        /**
         * @param resource $input
         * @param resource|null $output
         */
        public static function fromStreams($input, $output = null): Terminal {}

        public function getSize(): TerminalSize|false {}

        public function enableRawMode(): ModeToken {}

        public function restoreMode(?ModeToken $mode = null): bool {}

        public function readKey(
            ?\Time\Duration $timeout = null,
            ?\Time\Duration $sequenceTimeout = null,
        ): Key|string|false {}

        public function readSecret(): string {}
    }
}
