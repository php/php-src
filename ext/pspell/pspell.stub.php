<?php

/** @generate-function-entries */

final class PSpell {}
final class PSpellConfig {}

function pspell_new(string $language, string $spelling = "", string $jargon = "", string $encoding = "", int $mode = 0): PSpell|false {}

function pspell_new_personal(
    string $filename,
    string $language,
    string $spelling = "",
    string $jargon = "",
    string $encoding = "",
    int $mode = 0
): PSpell|false {}

function pspell_new_config(PSpellConfig $config): PSpell|false {}

function pspell_check(PSpell $dictionary, string $word): bool {}
function pspell_suggest(PSpell $dictionary, string $word): array|false {}
function pspell_store_replacement(PSpell $dictionary, string $misspelled, string $correct): bool {}
function pspell_add_to_personal(PSpell $dictionary, string $word): bool {}
function pspell_add_to_session(PSpell $dictionary, string $word): bool {}
function pspell_clear_session(PSpell $dictionary): bool {}
function pspell_save_wordlist(PSpell $dictionary): bool {}

function pspell_config_create(string $language, string $spelling = "", string $jargon = "", string $encoding = ""): PSpellConfig {}
function pspell_config_runtogether(PSpellConfig $config, bool $allow): bool {}
function pspell_config_mode(PSpellConfig $config, int $mode): bool {}
function pspell_config_ignore(PSpellConfig $config, int $min_length): bool {}
function pspell_config_personal(PSpellConfig $config, string $filename): bool {}
function pspell_config_dict_dir(PSpellConfig $config, string $directory): bool {}
function pspell_config_data_dir(PSpellConfig $config, string $directory): bool {}
function pspell_config_repl(PSpellConfig $config, string $filename): bool {}
function pspell_config_save_repl(PSpellConfig $config, bool $save): bool {}
