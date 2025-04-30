--TEST--
mbstring Unicode Data tests
--EXTENSIONS--
mbstring
--FILE--
<?php

echo "Char widths:\n";

print "ASCII (PHP): " .      mb_strwidth('PHP',    'UTF-8')    . "\n";

print "Vietnamese (Xin chào): " . mb_strwidth('Xin chào',    'UTF-8')    . "\n";

print "Traditional Chinese (你好): " . mb_strwidth('你好',    'UTF-8')    . "\n";

print "Sinhalese (අයේෂ්): " . mb_strwidth('අයේෂ්',    'UTF-8')    . "\n";

print "Emoji (\u{1F418}): " . mb_strwidth("\u{1F418}", 'UTF-8') . "\n";

// New in Unicode 15.0, width=2
print "Emoji (\u{1F6DC}): " . mb_strwidth("\u{1F6DC}", 'UTF-8') . "\n";

// Changed in Unicode 16.0, U+2630...U+2637 are wide
print "Emoji (\u{2630}): " . mb_strwidth("\u{2630}", 'UTF-8') . "\n";

echo "Char case changes:\n";

print "Upper(\u{019b}) = \u{a7dc} : ";
var_dump(mb_strtoupper("\u{019b}", 'UTF-8') === "\u{a7dc}");
?>
--EXPECT--
Char widths:
ASCII (PHP): 3
Vietnamese (Xin chào): 8
Traditional Chinese (你好): 4
Sinhalese (අයේෂ්): 5
Emoji (🐘): 2
Emoji (🛜): 2
Emoji (☰): 2
Char case changes:
Upper(ƛ) = Ƛ : bool(true)
