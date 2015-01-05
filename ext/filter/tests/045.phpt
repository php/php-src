--TEST--
Options must not be changed by filter_var()
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$a = array("flags"=>(string)FILTER_FLAG_ALLOW_HEX, "options" => array("min_range"=>"0", "max_range"=>"1024")); 
$ret = filter_var("0xff", FILTER_VALIDATE_INT, $a);
echo ($ret === 255 && $a["options"]["min_range"] === "0")?"ok\n":"bug\n";
echo ($ret === 255 && $a["options"]["max_range"] === "1024")?"ok\n":"bug\n";
echo ($ret === 255 && is_string($a["flags"]) && $a["flags"] == FILTER_FLAG_ALLOW_HEX)?"ok\n":"bug\n";
$a = (string)FILTER_FLAG_ALLOW_HEX;
$ret = filter_var("0xff", FILTER_VALIDATE_INT, $a);
echo ($ret === 255 && is_string($a) && $a == FILTER_FLAG_ALLOW_HEX)?"ok\n":"bug\n";
$a = array("test"=>array("filter"=>(string)FILTER_VALIDATE_INT, "flags"=>(string)FILTER_FLAG_ALLOW_HEX));
$ret = filter_var_array(array("test"=>"0xff"), $a);
echo ($ret["test"] === 255 && is_string($a["test"]["filter"]) && $a["test"]["filter"] == FILTER_VALIDATE_INT)?"ok\n":"bug\n";
echo ($ret["test"] === 255 && is_string($a["test"]["flags"]) && $a["test"]["flags"] == FILTER_FLAG_ALLOW_HEX)?"ok\n":"bug\n";
$a = array("test"=>(string)FILTER_VALIDATE_INT);
$ret = filter_var_array(array("test"=>"255"), $a);
echo ($ret["test"] === 255 && is_string($a["test"]) && $a["test"] == FILTER_VALIDATE_INT)?"ok\n":"bug\n";
?>
--EXPECT--
ok
ok
ok
ok
ok
ok
ok
