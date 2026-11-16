--TEST--
readline_info(): completion_append_character supports non-ASCII bytes
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (READLINE_LIB !== 'readline') die('skip readline only');
?>
--FILE--
<?php
readline_info('completion_append_character', '');
foreach (["\x7f", "\x80", "\xfftail", ''] as $value) {
    $old = readline_info('completion_append_character', $value);
    $current = readline_info('completion_append_character');
    $info = readline_info();
    printf("old=%s current=%s info=%s\n",
        bin2hex($old),
        bin2hex($current),
        bin2hex($info['completion_append_character'])
    );
}
?>
--EXPECT--
old= current=7f info=7f
old=7f current=80 info=80
old=80 current=ff info=ff
old=ff current= info=
