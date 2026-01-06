--TEST--
T_FROM in token_get_all vs method call
--EXTENSIONS--
tokenizer
--FILE--
<?php
$code1 = "<?php\nuse MyClass from Foo\\Bar;\n";
$tokens1 = token_get_all($code1);
foreach ($tokens1 as $t) {
    if (is_array($t) && $t[0] === T_FROM) {
        echo "T_FROM in use\n";
    }
}

$code2 = "<?php\n$db->from('users');\n";
$tokens2 = token_get_all($code2);
foreach ($tokens2 as $t) {
    if (is_array($t) && $t[0] === T_FROM) {
        echo "T_FROM in method\n";
    }
    if (is_array($t) && $t[0] === T_STRING && $t[1] === 'from') {
        echo "T_STRING from in method\n";
    }
}
?>
--EXPECT--
T_FROM in use
T_STRING from in method
