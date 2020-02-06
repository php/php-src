--TEST--
PMOPB-45-2007:PHP ext/filter Email Validation Vulnerability
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
    $var = "test@example.com\n";
    var_dump(filter_var($var, FILTER_VALIDATE_EMAIL));
?>
--EXPECT--
bool(false)
