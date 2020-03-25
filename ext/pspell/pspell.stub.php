<?php

function pspell_new(string $language, string $spelling = UNKNOWN, string $jargon = UNKNOWN, string $encoding = UNKNOWN, int $mode = 0): int|false {}

function pspell_new_personal(string $personal, string $language, string $spelling = UNKNOWN, string $jargon = UNKNOWN, string $encoding = UNKNOWN, $mode = 0): int|false {}

function pspell_new_config(int $config): int|false {}

function pspell_check(int $pspell, string $word): bool {}

function pspell_suggest(int $pspell, string $word): array|false {}

function pspell_store_replacement(int $pspell, string $misspell, string $correct): bool {}

function pspell_add_to_personal(int $pspell, string $word): bool {}

function pspell_add_to_session(int $pspell, string $word): bool {}

function pspell_clear_session(int $pspell): bool {}

function pspell_save_wordlist(int $pspell): bool {}

function pspell_config_create(string $language, string $spelling = UNKNOWN, string $jargon = UNKNOWN, string $encoding = UNKNOWN): int {}

function pspell_config_runtogether(int $conf, bool $runtogether): bool {}

function pspell_config_mode(int $conf, int $mode): bool {}

function pspell_config_ignore(int $conf, int $ignore): bool {}

function pspell_config_personal(int $conf, string $personal): bool {}

function pspell_config_dict_dir(int $conf, string $directory): bool {}

function pspell_config_data_dir(int $conf, string $directory): bool {}

function pspell_config_repl(int $conf, string $repl): bool {}

function pspell_config_save_repl(int $conf, bool $save): bool {}
