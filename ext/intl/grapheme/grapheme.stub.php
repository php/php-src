function grapheme_strlen(string $string, int $form = GRAPHEME_EXTR_COUNT): int|false {}
function grapheme_strpos(string $haystack, string $needle, int $offset = 0, int $form = GRAPHEME_EXTR_COUNT): int|false {}
function grapheme_stripos(string $haystack, string $needle, int $offset = 0, int $form = GRAPHEME_EXTR_COUNT): int|false {}
function grapheme_strrpos(string $haystack, string $needle, int $offset = 0, int $form = GRAPHEME_EXTR_COUNT): int|false {}
function grapheme_strripos(string $haystack, string $needle, int $offset = 0, int $form = GRAPHEME_EXTR_COUNT): int|false {}
function grapheme_substr(string $string, int $offset, ?int $length = null, int $form = GRAPHEME_EXTR_COUNT): string|false {}
function grapheme_strstr(string $haystack, string $needle, bool $before_needle = false, int $form = GRAPHEME_EXTR_COUNT): string|false {}
function grapheme_stristr(string $haystack, string $needle, bool $before_needle = false, int $form = GRAPHEME_EXTR_COUNT): string|false {}
function grapheme_extract(string $haystack, int $size, int $type = GRAPHEME_EXTR_COUNT, int $start = 0, int &$next = null, int $form = GRAPHEME_EXTR_COUNT): string|false {}
function grapheme_mask(string $string, string $mask_char = "*", int $offset = 0, ?int $length = null): string|false {}
