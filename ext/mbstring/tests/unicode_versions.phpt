--TEST--
mbstring Unicode Data tests
--EXTENSIONS--
mbstring
--FILE--
<?php

print "ASCII (PHP): " .      mb_strwidth('PHP',    'UTF-8')    . "\n";

print "Vietnamese (Xin chào): " . mb_strwidth('Xin chào',    'UTF-8')    . "\n";

print "Traditional Chinese (你好): " . mb_strwidth('你好',    'UTF-8')    . "\n";

print "Sinhalese (අයේෂ්): " . mb_strwidth('අයේෂ්',    'UTF-8')    . "\n";

print "Emoji (\u{1F418}): " . mb_strwidth("\u{1F418}", 'UTF-8') . "\n";

// New in Unicode 15.0, width=2
print "Emoji (\u{1F6DC}): " . mb_strwidth("\u{1F6DC}", 'UTF-8') . "\n";

?>
--EXPECT--
ASCII (PHP): 3
Vietnamese (Xin chào): 8
Traditional Chinese (你好): 4
Sinhalese (අයේෂ්): 5
Emoji (🐘): 2
Emoji (🛜): 2
