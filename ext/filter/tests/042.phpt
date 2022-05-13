--TEST--
Combination of strip & sanitize filters
--EXTENSIONS--
filter
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
--EXPECTF--
Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
XYZalert(/ext/filter+bypass/);ABC

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
XYZalert(/ext/filter+bypass/);ABC
