--TEST--
Bug #79668 (get_defined_functions(true) may miss functions)
--INI--
disable_functions=sha1_file,password_hash
--FILE--
<?php
$df = get_defined_functions(true);
foreach (['sha1', 'sha1_file', 'hash', 'password_hash'] as $funcname) {
    var_dump(in_array($funcname, $df['internal'], true));
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
