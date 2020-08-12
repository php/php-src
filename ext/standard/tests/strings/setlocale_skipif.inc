<?php
function require_locale(int $category, ...$locales) {
    if (setlocale($category, "This locale definitely does not exist")) {
        die("skip Cannot determine available locales");
    }
    if (!setlocale($category, ...$locales)) {
        $localesStr = implode(", ", $locales);
        die("skip Locales not available: $localesStr");
    }
}
