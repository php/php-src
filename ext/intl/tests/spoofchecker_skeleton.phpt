--TEST--
Spoofchecker::getSkeleton()
--EXTENSIONS--
intl
--FILE--
<?php
$checker = new Spoofchecker();

var_dump($checker->getSkeleton(""));
var_dump($checker->getSkeleton("abc"));

try {
    $checker->getSkeleton();
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Cyrillic es is confusable with Latin c, so both share a skeleton. */
$latin = "c";
$cyrillic = "\u{0441}";
var_dump($checker->getSkeleton($latin) === $checker->getSkeleton($cyrillic));
var_dump($checker->areConfusable($latin, $cyrillic));

/* Unrelated identifiers must not collapse onto the same skeleton. */
var_dump($checker->getSkeleton("abc") === $checker->getSkeleton("xyz"));

/* A skeleton is its own skeleton. */
var_dump($checker->getSkeleton($checker->getSkeleton($cyrillic)) === $checker->getSkeleton($cyrillic));

/* The mapping may expand, which exercises the preflighted result buffer. */
var_dump($checker->getSkeleton("\u{FB01}"));

/* Skeletons are binary safe. */
var_dump(bin2hex($checker->getSkeleton("a\0b")));

/* The skeleton is derived from the confusable data only, never from the
   checker configuration. */
$configured = new Spoofchecker();
$configured->setChecks(Spoofchecker::SINGLE_SCRIPT);
$configured->setRestrictionLevel(Spoofchecker::ASCII);
$configured->setAllowedChars("[a-z]");
var_dump($configured->getSkeleton($cyrillic) === $checker->getSkeleton($cyrillic));

/* Ill-formed UTF-8 is rejected instead of being substituted. */
foreach (["\x80", "\xC3", "\xE2\x82", "\xED\xA0\x80", "abc\xFF"] as $malformed) {
    var_dump($checker->getSkeleton($malformed));
    var_dump(intl_get_error_code() === U_INVALID_CHAR_FOUND);
}

/* The error state does not leak into the next call. */
var_dump($checker->getSkeleton("abc"));
var_dump(intl_get_error_code() === U_ZERO_ERROR);
?>
--EXPECT--
string(0) ""
string(3) "abc"
ArgumentCountError: Spoofchecker::getSkeleton() expects exactly 1 argument, 0 given
bool(true)
bool(true)
bool(false)
bool(true)
string(2) "fi"
string(6) "610062"
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
string(3) "abc"
bool(true)
