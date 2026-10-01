--TEST--
Spoofchecker::getBidiSkeleton()
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '74.0') < 0) die('skip for ICU >= 74.0'); ?>
--FILE--
<?php
$checker = new Spoofchecker();

var_dump($checker->getBidiSkeleton(Spoofchecker::LTR, ""));

try {
    $checker->getBidiSkeleton(Spoofchecker::RTL + 1, "a");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* These identifiers are confusable in a left-to-right context only. */
var_dump(
    $checker->getBidiSkeleton(Spoofchecker::LTR, "A1\u{05D0}") ===
    $checker->getBidiSkeleton(Spoofchecker::LTR, "A\u{05D0}1")
);
var_dump(
    $checker->getBidiSkeleton(Spoofchecker::RTL, "A1\u{05D0}") ===
    $checker->getBidiSkeleton(Spoofchecker::RTL, "A\u{05D0}1")
);

/* These identifiers are confusable in a right-to-left context only. */
var_dump(
    $checker->getBidiSkeleton(Spoofchecker::LTR, "\u{05D0}A_1") ===
    $checker->getBidiSkeleton(Spoofchecker::LTR, "\u{05D0}1_A")
);
var_dump(
    $checker->getBidiSkeleton(Spoofchecker::RTL, "\u{05D0}A_1") ===
    $checker->getBidiSkeleton(Spoofchecker::RTL, "\u{05D0}1_A")
);

var_dump($checker->getBidiSkeleton(Spoofchecker::LTR, "\x80"));
var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
?>
--EXPECT--
string(0) ""
ValueError: Spoofchecker::getBidiSkeleton(): Argument #1 ($direction) must be either Spoofchecker::LTR or Spoofchecker::RTL
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
