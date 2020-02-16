<?php

function tidy_parse_string(string $input, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN): tidy|false {}

function tidy_get_error_buffer(tidy $object): string|false {}

function tidy_get_output(tidy $object): string {}

function tidy_parse_file(string $file, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN, bool $use_include_path = false): tidy|false {}

function tidy_clean_repair(tidy $object): bool {}

function tidy_repair_string(string $data, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN): string|false {}

function tidy_repair_file(string $filename, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN, bool $use_include_path = false): string|false {}

function tidy_diagnose(tidy $object): bool {}

function tidy_get_release(): string {}

#if HAVE_TIDYOPTGETDOC
function tidy_get_opt_doc(tidy $object, string $optname): string|false {}
#endif

function tidy_get_config(tidy $object): array {}

function tidy_get_status(tidy $object): int {}

function tidy_get_html_ver(tidy $object): int {}

function tidy_is_xhtml(tidy $object): bool {}

function tidy_is_xml(tidy $object): bool {}

function tidy_error_count(tidy $object): int {}

function tidy_warning_count(tidy $object): int {}

function tidy_access_count(tidy $object): int {}

function tidy_config_count(tidy $object): int {}

function tidy_getopt(tidy $object, string $option): string|int|bool {}

function tidy_get_root(tidy $object): ?tidyNode {}

function tidy_get_html(tidy $object): ?tidyNode {}

function tidy_get_head(tidy $object): ?tidyNode {}

function tidy_get_body(tidy $tidy): ?tidyNode {}

class tidy
{
    public function __construct(string $filename = UNKNOWN, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN, bool $use_include_path = false) {}

    /** @return string|int|bool */
    public function getOpt(string $option) {}

    public function cleanRepair(): bool {}

    public function parseFile(string $file, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN, bool $use_include_path = false): bool {}

    public function parseString(string $input, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN): bool {}

    public function repairString(string $data, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN): bool {}

    public function repairFile(string $filename, array|string $config_options = UNKNOWN, string $encoding = UNKNOWN, bool $use_include_path = false): bool {}

    public function diagnose(): bool {}

    public function getRelease(): string {}

    public function getConfig(): array {}

    public function getStatus(): int {}

    public function getHtmlVer(): int {}

#if HAVE_TIDYOPTGETDOC
    /** @return string|false */
    public function getOptDoc(string $optname) {}
#endif

    public function isXhtml(): bool {}

    public function isXml(): bool {}

    public function root(): ?tidyNode {}

    public function head(): ?tidyNode {}

    public function html(): ?tidyNode {}

    public function body(): ?tidyNode {}
}

class tidyNode
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
