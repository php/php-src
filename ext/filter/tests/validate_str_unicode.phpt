--TEST--
FILTER_VALIDATE_STR: Unicode maximal subpart validation (Table 3-11 reference)
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
/**
 * These tests verify that FILTER_VALIDATE_STR correctly counts invalid or truncated UTF-8 sequences
 * as single characters, per Unicode Standard Section 3.9.6: "U+FFFD Substitution of Maximal Subparts"
 * (https://www.unicode.org/versions/Unicode16.0.0/core-spec/chapter-3/#G66453).
 *
 * The filter only performs validation and does not modify the original data.
 * As long as the length constraint is satisfied, the input byte string is returned.
 *
 * Example 1 (single invalid byte):
 *   Input: 61 80 ("a" followed by invalid \x80)
 *   Validation: 2 code points (min_range = 2, max_range = 2) → returns original input
 *   Expected hex: 6180
 *
 * Example 2 (Table 3-11 inspired):
 *   Input: E1 80 E2 F0 91 92 F1 BF 41
 *   Validation: 5 code points (max_range = 5) → returns original input
 *   Expected hex: e180e2f09192f1bf41
 *
 */

$options1 = ['options' => ['min_range' => 2, 'max_range' => 2]];
$options2 = ['options' => ['max_range' => 5]];

echo bin2hex(filter_var("a\x80", FILTER_VALIDATE_STRLEN, $options1)), "\n";
echo bin2hex(filter_var("\xE1\x80\xE2\xF0\x91\x92\xF1\xBF\x41", FILTER_VALIDATE_STRLEN, $options2)), "\n";
?>
--EXPECT--
6180
e180e2f09192f1bf41
