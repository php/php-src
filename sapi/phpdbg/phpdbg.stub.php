<?php

/** @generate-class-entries */

/**
 * @var string
 * @cvalue PHPDBG_VERSION
 */
const PHPDBG_VERSION = UNKNOWN;

/**
 * @var int
 * @cvalue PHPDBG_COLOR_PROMPT
 */
const PHPDBG_COLOR_PROMPT = UNKNOWN;
/**
 * @var int
 * @cvalue PHPDBG_COLOR_NOTICE
 */
const PHPDBG_COLOR_NOTICE = UNKNOWN;
/**
 * @var int
 * @cvalue PHPDBG_COLOR_ERROR
 */
const PHPDBG_COLOR_ERROR = UNKNOWN;

function phpdbg_break_next(): void {}

function phpdbg_break_file(string $file, int $line): void {}

function phpdbg_break_method(string $class, string $method): void {}

function phpdbg_break_function(string $function): void {}

function phpdbg_color(int $element, string $color): void {}

function phpdbg_prompt(string $string): void {}

function phpdbg_exec(string $context): string|bool {}

function phpdbg_clear(): void {}

function phpdbg_start_oplog(): void {}

function phpdbg_end_oplog(array $options = []): ?array {}

function phpdbg_get_executable(array $options = []): array {}
