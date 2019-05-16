--TEST--
Bug #76437 (token_get_all with TOKEN_PARSE flag fails to recognise close tag)
--SKIPIF--
<?php if (!extension_loaded("tokenizer")) print "skip"; ?>
--FILE--
<?php
$tests = [
    ['<?=$a?>', 0],
    ['<?php echo 2; ?>', 6],
    ["<?php echo 2; ?>\n", 6],
];
foreach ($tests as [$code, $index]) {
    $open_tag1 = token_get_all($code)[$index];
    $open_tag2 = token_get_all($code, TOKEN_PARSE)[$index];
    echo token_name($open_tag1[0]), ": \"$open_tag1[1]\" on line $open_tag1[2]\n";
    var_dump($open_tag1 === $open_tag2);
}
?>
--EXPECT--
T_OPEN_TAG_WITH_ECHO: "<?=" on line 1
bool(true)
T_CLOSE_TAG: "?>" on line 1
bool(true)
T_CLOSE_TAG: "?>
" on line 1
bool(true)
