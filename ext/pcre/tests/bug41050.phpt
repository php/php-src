--TEST--
Bug #41050 (pcre 7.0 regression)
--FILE--
<?php
// by legolas558

$regex = '/(insert|drop|create|select|delete|update)([^;\']*('."('[^']*')+".')?)*(;|$)/i';

$sql = 'SELECT * FROM #__components';

if (preg_match($regex,$sql, $m)) echo 'matched';
else echo 'not matched';

print_r($m);

?>
--EXPECT--
matchedArray
(
    [0] => SELECT * FROM #__components
    [1] => SELECT
    [2] => 
    [3] => 
    [4] => 
    [5] => 
)
