--TEST--
fputcsv(): bug #43225 (fputcsv incorrectly handles cells ending in \ followed by ")
--FILE--
<?php

$row = array(
    'a\\"',
    'bbb',
);

$file = dirname(__FILE__) . 'fgetcsv_bug43225.csv';
$fp = fopen($file, 'w');
fputcsv($fp, $row);
fclose($fp);
readfile($file);
unlink($file);

?>
--EXPECT--
"a\""",bbb
