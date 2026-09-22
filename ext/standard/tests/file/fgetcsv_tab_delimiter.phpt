--TEST--
fgetcsv() with tab delimited fields (BUG #8258)
--FILE--
<?php

$file = __DIR__ . '/fgetcsv_tab_delimiter.data';
$fp = fopen($file,"r");
while ($a = fgetcsv($fp, 100, "\t", escape: '')) {
    echo join(",",$a)."\n";
}
fclose($fp);
?>
--EXPECT--
name,value,comment
true,1,boolean true
false,0,boolean false
empty,,nothing
