--TEST--
strnat(case)cmp(): potential OOB access for unterminated strings
--SKIPIF--
<?php
if (!function_exists('zend_create_unterminated_string')) die('skip zend_test extension not available');
?>
--FILE--
<?php
$a = zend_create_unterminated_string('333');
$b = zend_create_unterminated_string('333 ');
var_dump(
    strnatcmp($a, $b),
    strnatcasecmp($b, $a)
);
zend_terminate_string($a);
zend_terminate_string($b);
?>
--EXPECT--
int(-1)
int(1)
