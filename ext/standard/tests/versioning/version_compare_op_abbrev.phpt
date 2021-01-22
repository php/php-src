--TEST--
version_compare() no longer supports operator abbreviations
--FILE--
<?php
$abbrevs = ['', 'l', 'g', 'e', '!', 'n'];
foreach ($abbrevs as $op) {
    try {
        version_compare('1', '2', $op);
        echo "'$op' succeeded\n";
    } catch (ValueError $err) {
        echo "'$op' failed\n";
    }
}
?>
--EXPECT--
'' failed
'l' failed
'g' failed
'e' failed
'!' failed
'n' failed
