--TEST--
Bug #26696 (crash in switch() when string index is used)
--FILE--
<?php
$str = 'asdd/?';
$len = strlen($str);
for ($i = 0; $i < $len; $i++) {
    switch ($str[$i]) {
    case '?':
        echo "?+\n";
        break;
    default:
        echo $str[$i].'-';
        break;
    }
}

?>
--EXPECT--
a-s-d-d-/-?+
