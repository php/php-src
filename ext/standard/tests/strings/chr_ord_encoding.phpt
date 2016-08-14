--TEST--
Test chr() and ord() function : encoding option
--FILE--
<?php
var_dump(
  // HIRAGANA LETTER A
  "\u{3042}" === chr(0x3042, "UTF-8"),
  "\x82\xa0" === chr(0x82a0, "Shift_JIS"),

  // invalid codepoint
  "\u{FFFD}" === chr(-1, "UTF-8"),
  "?" === chr(-1, "Shift_JIS")
);
var_dump(
  // HIRAGANA LETTER A
  0x3042 === ord("\u{3042}", "UTF-8"),
  0x82a0 === ord("\x82\xa0", "Shift_JIS"),

  // ill-formed byte sequences
  0xfffd === ord("\xff", "UTF-8"),
  0x3f === ord("\x80", "Shift_JIS")
);

?>
===DONE===
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
