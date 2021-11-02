<?php

/** @generate-class-entries */

function phpdbg_break_next(): void {}

function phpdbg_break_file(string $file, int $line): void {}

function phpdbg_break_method(string $class, string $method): void {}

function phpdbg_break_function(string $function): void {}

function phpdbg_watch(string $pattern, null|array|object $base = null, ?callable $callback = null): ?int {}

function phpdbg_watch_recursive(string $pattern, null|array|object $base = null, ?callable $callback = null): ?int {}

function phpdbg_unwatch(int $watcher_id): void {}

function phpdbg_color(int $element, string $color): void {}

function phpdbg_prompt(string $string): void {}

function phpdbg_exec(string $context): string|bool {}

function phpdbg_clear(): void {}

function phpdbg_start_oplog(): void {}

function phpdbg_end_oplog(array $options = []): ?array {}

function phpdbg_get_executable(array $options = []): array {}

enum PhpdbgWatchModification {
    case ADDED;
    case UPDATED;
    case REFCOUNT;
    case REMOVED;
}
