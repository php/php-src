--TEST--
Bigint parsing tests
--SKIPIF--
<?php if (!extension_loaded('tokenizer')) { die('skip requires tokenizer'); } ?>
--FILE--
<?php
foreach (token_get_all('<?php return 1111111111111111111111111111111111111111111111111239n+012_345n+1n;?>') as $token) {
    if (is_string($token)) {
        printf("%s\n", $token);
        continue;
    }
    printf("%s: '%s'\n", token_name($token[0]), $token[1]);
}
?>
--EXPECT--
T_OPEN_TAG: '<?php '
T_RETURN: 'return'
T_WHITESPACE: ' '
T_BIGINT: '1111111111111111111111111111111111111111111111111239n'
+
T_BIGINT: '012_345n'
+
T_BIGINT: '1n'
;
T_CLOSE_TAG: '?>'