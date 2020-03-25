<?php

function readline(?string $prompt = null): string|false {}

/** @return mixed */
function readline_info(string $varname = UNKNOWN, string $newvalue = UNKNOWN) {}

function readline_add_history(string $prompt): bool {}

function readline_clear_history(): bool {}

#ifdef HAVE_HISTORY_LIST
function readline_list_history(): array {}
#endif

function readline_read_history(string $filename = UNKNOWN): bool {}

function readline_write_history(string $filename = UNKNOWN): bool {}

/**
 * @param callable $funcname
 */
function readline_completion_function($funcname): bool {}


#if HAVE_RL_CALLBACK_READ_CHAR
/**
 * @param callable $callback
 */
function readline_callback_handler_install(string $prompt, $callback): bool {}

function readline_callback_read_char(): void {}

function readline_callback_handler_remove(): bool {}

function readline_redisplay(): void {}

#if HAVE_RL_ON_NEW_LINE
function readline_on_new_line(): void {}
#endif
#endif
