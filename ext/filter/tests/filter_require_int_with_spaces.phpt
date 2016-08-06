--TEST--
Integer validation with spaces
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
precision=14
--FILE--
<?php 
$vals = array(
"	
 ",
" ",
" 123",
" 123.01 ",
);

foreach ($vals as $var) {
    try {
        var_dump(filter_require_var($var, FILTER_VALIDATE_FLOAT));
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }
}
?>
--EXPECT--
string(46) "Filter validated value became empty after trim"
string(46) "Filter validated value became empty after trim"
string(52) "Float validation: Invalid float format. Found spaces"
string(52) "Float validation: Invalid float format. Found spaces"
