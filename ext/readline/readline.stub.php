<?php

/** @generate-class-entries */

/**
 * @var string
 * @cvalue READLINE_LIB
 */
const READLINE_LIB = UNKNOWN;

function readline(?string $prompt = null): string|false {}

/** @param int|string|bool|null $value */
function readline_info(?string $var_name = null, $value = null): mixed {}

function readline_add_history(string $prompt): bool {}

function readline_clear_history(): bool {}

#ifdef HAVE_HISTORY_LIST
/**
 * @return array<int, string>
 * @refcount 1
 */
function readline_list_history(): array {}
#endif

function readline_read_history(?string $filename = null): bool {}

function readline_write_history(?string $filename = null): bool {}

function readline_completion_function(callable $callback): bool {}


#if HAVE_RL_CALLBACK_READ_CHAR
function readline_callback_handler_install(string $prompt, callable $callback): bool {}

function readline_callback_read_char(): void {}

function readline_callback_handler_remove(): bool {}

function readline_redisplay(): void {}

#if HAVE_RL_ON_NEW_LINE
function readline_on_new_line(): void {}
#endif
#endif
