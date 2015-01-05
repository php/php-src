--TEST--
Bug #21966 (date() or mktime() returning bad value for mktime month param of '2')
--INI--
date.timezone=Europe/London
--FILE--
<?php
echo '27/3/04 = ' . strval(mktime(0,0,0,3,27,2004)) . "\n";   // 1080345600
echo '28/3/04 = ' . strval(mktime(0,0,0,3,28,2004)) . "\n";   // -3662  - should be 108043200
echo '28/3/04 = ' . strval(mktime(2,0,0,3,28,2004)) . "\n";   // 1080435600
echo '29/3/04 = ' . strval(mktime(0,0,0,3,29,2004)) . "\n";   // 1080514800
echo '30/3/04 = ' . strval(mktime(0,0,0,3,30,2004)) . "\n";   // 1080601200

?>
--EXPECT--
27/3/04 = 1080345600
28/3/04 = 1080432000
28/3/04 = 1080435600
29/3/04 = 1080514800
30/3/04 = 1080601200
