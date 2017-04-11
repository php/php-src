--TEST--
Bug #43225: fputcsv incorrectly handles cells ending in \ followed by "
--FILE--
<?php
fputcsv(STDOUT, ['a\\"', 'bbb']);
--EXPECT--
"a\""",bbb
