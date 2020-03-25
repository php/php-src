--TEST--
fgetcsv() with tab delimited fields (BUG #8258)
--FILE--
<?php
chdir(__DIR__);
$fp=fopen("004.data","r");
while($a=fgetcsv($fp,100,"\t")) {
    echo join(",",$a)."\n";
}
fclose($fp);
?>
--EXPECT--
name,value,comment
true,1,boolean true
false,0,boolean false
empty,,nothing
