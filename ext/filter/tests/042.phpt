--TEST--
Combination of strip & sanitize filters
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$var = 'XYZ< script>alert(/ext/filter+bypass/);< /script>ABC';
$a = filter_var($var, FILTER_SANITIZE_STRING, array("flags" => FILTER_FLAG_STRIP_LOW));
echo $a . "\n";

$var = 'XYZ<
script>alert(/ext/filter+bypass/);<
/script>ABC';
$a = filter_var($var, FILTER_SANITIZE_STRING, array("flags" => FILTER_FLAG_STRIP_LOW));
echo $a . "\n";
?>
--EXPECT--
XYZalert(/ext/filter+bypass/);ABC
XYZalert(/ext/filter+bypass/);ABC
