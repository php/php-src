<?php

/** @generate-class-entries */

function tidy_parse_string(string $string, array|string|null $config = null, ?string $encoding = null): tidy|false {}

function tidy_get_error_buffer(tidy $tidy): string|false {}

function tidy_get_output(tidy $tidy): string {}

function tidy_parse_file(string $filename, array|string|null $config = null, ?string $encoding = null, bool $useIncludePath = false): tidy|false {}

function tidy_clean_repair(tidy $tidy): bool {}

function tidy_repair_string(string $string, array|string|null $config = null, ?string $encoding = null): string|false {}

function tidy_repair_file(string $filename, array|string|null $config = null, ?string $encoding = null, bool $useIncludePath = false): string|false {}

function tidy_diagnose(tidy $tidy): bool {}

function tidy_get_release(): string {}

#ifdef HAVE_TIDYOPTGETDOC
function tidy_get_opt_doc(tidy $tidy, string $option): string|false {}
#endif

function tidy_get_config(tidy $tidy): array {}

function tidy_get_status(tidy $tidy): int {}

function tidy_get_html_ver(tidy $tidy): int {}

function tidy_is_xhtml(tidy $tidy): bool {}

function tidy_is_xml(tidy $tidy): bool {}

function tidy_error_count(tidy $tidy): int {}

function tidy_warning_count(tidy $tidy): int {}

function tidy_access_count(tidy $tidy): int {}

function tidy_config_count(tidy $tidy): int {}

function tidy_getopt(tidy $tidy, string $option): string|int|bool {}

function tidy_get_root(tidy $tidy): ?tidyNode {}

function tidy_get_html(tidy $tidy): ?tidyNode {}

function tidy_get_head(tidy $tidy): ?tidyNode {}

function tidy_get_body(tidy $tidy): ?tidyNode {}

class tidy
{
    public function __construct(?string $filename = null, array|string|null $config = null, ?string $encoding = null, bool $useIncludePath = false) {}

    /**
     * @tentative-return-type
     * @alias tidy_getopt
     */
    public function getOpt(string $option): string|int|bool {}

    /**
     * @tentative-return-type
     * @alias tidy_clean_repair
     */
    public function cleanRepair(): bool {}

    /** @tentative-return-type */
    public function parseFile(string $filename, array|string|null $config = null, ?string $encoding = null, bool $useIncludePath = false): bool {}

    /** @tentative-return-type */
    public function parseString(string $string, array|string|null $config = null, ?string $encoding = null): bool {}

    /**
     * @tentative-return-type
     * @alias tidy_repair_string
     */
    public static function repairString(string $string, array|string|null $config = null, ?string $encoding = null): string|false {}

    /**
     * @tentative-return-type
     * @alias tidy_repair_file
     */
    public static function repairFile(string $filename, array|string|null $config = null, ?string $encoding = null, bool $useIncludePath = false): string|false {}

    /**
     * @tentative-return-type
     * @alias tidy_diagnose
     */
    public function diagnose(): bool {}

    /**
     * @tentative-return-type
     * @alias tidy_get_release
     */
    public function getRelease(): string {}

    /**
     * @tentative-return-type
     * @alias tidy_get_config
     */
    public function getConfig(): array {}

    /**
     * @tentative-return-type
     * @alias tidy_get_status
     */
    public function getStatus(): int {}

    /**
     * @tentative-return-type
     * @alias tidy_get_html_ver
     */
    public function getHtmlVer(): int {}

#ifdef HAVE_TIDYOPTGETDOC
    /**
     * @tentative-return-type
     * @alias tidy_get_opt_doc
     */
    public function getOptDoc(string $option): string|false {}
#endif

    /**
     * @tentative-return-type
     * @alias tidy_is_xhtml
     */
    public function isXhtml(): bool {}

    /**
     * @tentative-return-type
     * @alias tidy_is_xml
     */
    public function isXml(): bool {}

    /**
     * @tentative-return-type
     * @alias tidy_get_root
     */
    public function root(): ?tidyNode {}

    /**
     * @tentative-return-type
     * @alias tidy_get_head
     */
    public function head(): ?tidyNode {}

    /**
     * @tentative-return-type
     * @alias tidy_get_html
     */
    public function html(): ?tidyNode {}

    /**
     * @tentative-return-type
     * @alias tidy_get_body
     */
    public function body(): ?tidyNode {}
}

final class tidyNode
{
    private function __construct() {}

    public function hasChildren(): bool {}

    public function hasSiblings(): bool {}

    public function isComment(): bool {}

    public function isHtml(): bool {}

    public function isText(): bool {}

    public function isJste(): bool {}

    public function isAsp(): bool {}

    public function isPhp(): bool {}

    public function getParent(): ?tidyNode {}
}
