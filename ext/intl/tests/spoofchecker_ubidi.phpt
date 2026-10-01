--TEST--
Spoofchecker::areBidiConfusable() checks if strings are confusable in a given direction.
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '74.0') < 0) die('skip for ICU >= 74.0'); ?>
--FILE--
<?php
$s = new Spoofchecker();

try {
    $s->areBidiConfusable(Spoofchecker::RTL + 1, "a", "a");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* "A1<aleph>" and "A<aleph>1" both display as "A1<aleph>" in a left to right
 * context, but differ in a right to left one. */
var_dump($s->areBidiConfusable(Spoofchecker::LTR, "A1\u{05D0}", "A\u{05D0}1"));
var_dump($s->areBidiConfusable(Spoofchecker::RTL, "A1\u{05D0}", "A\u{05D0}1"));

/* Mirror case: confusable in a right to left context only. */
var_dump($s->areBidiConfusable(Spoofchecker::LTR, "\u{05D0}A_1", "\u{05D0}1_A"));
var_dump($s->areBidiConfusable(Spoofchecker::RTL, "\u{05D0}A_1", "\u{05D0}1_A"));

/* Neither direction reorders these into each other. */
var_dump($s->areBidiConfusable(Spoofchecker::LTR, "Mark_", "_Mark"));
var_dump($s->areBidiConfusable(Spoofchecker::RTL, "Mark_", "_Mark"));

/* areConfusable() ignores the text direction and misses both cases above. */
var_dump($s->areConfusable("A1\u{05D0}", "A\u{05D0}1"));
var_dump($s->areConfusable("\u{05D0}A_1", "\u{05D0}1_A"));

$errorCode = null;
var_dump($s->areBidiConfusable(Spoofchecker::LTR, "A1\u{05D0}", "A\u{05D0}1", $errorCode));
var_dump($errorCode === Spoofchecker::MIXED_SCRIPT_CONFUSABLE);

var_dump($s->areBidiConfusable(Spoofchecker::LTR, "Mark_", "_Mark", $errorCode));
var_dump($errorCode);
?>
--EXPECT--
ValueError: Spoofchecker::areBidiConfusable(): Argument #1 ($direction) must be either Spoofchecker::LTR or Spoofchecker::RTL
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
int(0)
