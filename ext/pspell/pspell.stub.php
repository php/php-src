<?php

/** @generate-class-entries */

namespace PSpell {
    /**
     * @strict-properties
     * @not-serializable
     */
    final class Dictionary {}

    /**
     * @strict-properties
     * @not-serializable
     */
    final class Config {}
}

namespace {
    /**
     * @var int
     * @cvalue PSPELL_FAST
     */
    const PSPELL_FAST = UNKNOWN;
    /**
     * @var int
     * @cvalue PSPELL_NORMAL
     */
    const PSPELL_NORMAL = UNKNOWN;
    /**
     * @var int
     * @cvalue PSPELL_BAD_SPELLERS
     */
    const PSPELL_BAD_SPELLERS = UNKNOWN;
    /**
     * @var int
     * @cvalue PSPELL_RUN_TOGETHER
     */
    const PSPELL_RUN_TOGETHER = UNKNOWN;

    function pspell_new(string $language, string $spelling = "", string $jargon = "", string $encoding = "", int $mode = 0): PSpell\Dictionary|false {}

    function pspell_new_personal(
        string $filename,
        string $language,
        string $spelling = "",
        string $jargon = "",
        string $encoding = "",
        int $mode = 0
    ): PSpell\Dictionary|false {}

    function pspell_new_config(PSpell\Config $config): PSpell\Dictionary|false {}

    function pspell_check(PSpell\Dictionary $dictionary, string $word): bool {}

    /**
     * @return array<int, string>|false
     * @refcount 1
     */
    function pspell_suggest(PSpell\Dictionary $dictionary, string $word): array|false {}
    function pspell_store_replacement(PSpell\Dictionary $dictionary, string $misspelled, string $correct): bool {}
    function pspell_add_to_personal(PSpell\Dictionary $dictionary, string $word): bool {}
    function pspell_add_to_session(PSpell\Dictionary $dictionary, string $word): bool {}
    function pspell_clear_session(PSpell\Dictionary $dictionary): bool {}
    function pspell_save_wordlist(PSpell\Dictionary $dictionary): bool {}

    function pspell_config_create(string $language, string $spelling = "", string $jargon = "", string $encoding = ""): PSpell\Config {}
    function pspell_config_runtogether(PSpell\Config $config, bool $allow): bool {}
    function pspell_config_mode(PSpell\Config $config, int $mode): bool {}
    function pspell_config_ignore(PSpell\Config $config, int $min_length): bool {}
    function pspell_config_personal(PSpell\Config $config, string $filename): bool {}
    function pspell_config_dict_dir(PSpell\Config $config, string $directory): bool {}
    function pspell_config_data_dir(PSpell\Config $config, string $directory): bool {}
    function pspell_config_repl(PSpell\Config $config, string $filename): bool {}
    function pspell_config_save_repl(PSpell\Config $config, bool $save): bool {}

}
