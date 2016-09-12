--TEST--
mb_at()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
mb_internal_encoding('UTF-8');

// U+1D400: MATHEMATICAL BOLD CAPITAL A
$str = "\u{1D400}\u{1D401}\u{1D402}";

var_dump(
    "\u{1D400}" === mb_at($str, 0),
    "\u{1D402}" === mb_at($str, -1),
    "" === mb_at($str, 4),
    0x1D400 === mb_codepoint_at($str, 0),
    0x1D402 === mb_codepoint_at($str, -1),
    0 === mb_codepoint_at($str, 4)
);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
