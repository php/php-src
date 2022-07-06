<?php

/** @generate-function-entries */

function pspell_new(string $language, string $spelling = "", string $jargon = "", string $encoding = "", int $mode = 0): int|false {}

function pspell_new_personal(
    string $filename,
    string $language,
    string $spelling = "",
    string $jargon = "",
    string $encoding = "",
    int $mode = 0
): int|false {}

function pspell_new_config(int $config): int|false {}

function pspell_check(int $dictionary, string $word): bool {}

function pspell_suggest(int $dictionary, string $word): array|false {}

function pspell_store_replacement(int $dictionary, string $misspelled, string $correct): bool {}

function pspell_add_to_personal(int $dictionary, string $word): bool {}

function pspell_add_to_session(int $dictionary, string $word): bool {}

function pspell_clear_session(int $dictionary): bool {}

function pspell_save_wordlist(int $dictionary): bool {}

function pspell_config_create(string $language, string $spelling = "", string $jargon = "", string $encoding = ""): int {}

function pspell_config_runtogether(int $config, bool $allow): bool {}

function pspell_config_mode(int $config, int $mode): bool {}

function pspell_config_ignore(int $config, int $min_length): bool {}

function pspell_config_personal(int $config, string $filename): bool {}

function pspell_config_dict_dir(int $config, string $directory): bool {}

function pspell_config_data_dir(int $config, string $directory): bool {}

function pspell_config_repl(int $config, string $filename): bool {}

function pspell_config_save_repl(int $config, bool $save): bool {}
