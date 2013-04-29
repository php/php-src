--TEST--
grapheme_substr() - Bug55562 - grapheme_substr() returns false if length parameter is to large
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
var_dump(
  grapheme_substr('FOK', 1, 20), // expected: OK
  grapheme_substr('한국어', 1, 20) //expected: 국어
);
--EXPECT--
string(2) "OK"
string(6) "국어"
