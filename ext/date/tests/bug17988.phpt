--TEST--
Bug #17988 (strtotime handling of postgresql timestamps)
--INI--
date.timezone=GMT
--FILE--
<?php
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728 GMT"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728 EDT"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728-00"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728+00"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728-04"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728+04"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728-0300"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728+0300"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728-0330"))."\n";
echo gmdate('Y-m-d H:i:s', strtotime("2002-06-25 14:18:48.543728+0330"))."\n";
?>
--EXPECT--
2002-06-25 14:18:48
2002-06-25 14:18:48
2002-06-25 18:18:48
2002-06-25 14:18:48
2002-06-25 14:18:48
2002-06-25 18:18:48
2002-06-25 10:18:48
2002-06-25 17:18:48
2002-06-25 11:18:48
2002-06-25 17:48:48
2002-06-25 10:48:48
