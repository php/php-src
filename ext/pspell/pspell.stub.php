<?php

/** @generate-function-entries */

function pspell_new(string $language, string $spelling = "", string $jargon = "", string $encoding = "", int $mode = 0): int|false {}

function pspell_new_personal(
    string $personal_filename,
    string $language,
    string $spelling = "",
    string $jargon = "",
    string $encoding = "",
    int $mode = 0
): int|false {}

function pspell_new_config(int $config_id): int|false {}

function pspell_check(int $dictionary_id, string $word): bool {}

function pspell_suggest(int $dictionary_id, string $word): array|false {}

function pspell_store_replacement(int $dictionary_id, string $misspelled, string $correct): bool {}

function pspell_add_to_personal(int $dictionary_id, string $word): bool {}

function pspell_add_to_session(int $dictionary_id, string $word): bool {}

function pspell_clear_session(int $dictionary_id): bool {}

function pspell_save_wordlist(int $dictionary_id): bool {}

function pspell_config_create(string $language, string $spelling = "", string $jargon = "", string $encoding = ""): int {}

function pspell_config_runtogether(int $config_id, bool $allow): bool {}

function pspell_config_mode(int $config_id, int $mode): bool {}

function pspell_config_ignore(int $config_id, int $minimum_length): bool {}

function pspell_config_personal(int $config_id, string $filename): bool {}

function pspell_config_dict_dir(int $config_id, string $directory): bool {}

function pspell_config_data_dir(int $config_id, string $directory): bool {}

function pspell_config_repl(int $config_id, string $filename): bool {}

function pspell_config_save_repl(int $dictionary_id, bool $save): bool {}
