--TEST--
readline_info(): pending_input supports non-ASCII bytes
--EXTENSIONS--
readline
--SKIPIF--
<?php
if (READLINE_LIB !== 'readline') die('skip readline only');
?>
--FILE--
<?php
readline_info('pending_input', '');
foreach (["\x7f", "\x80", "\xfe", "\xfftail", ''] as $value) {
    $old = readline_info('pending_input', $value);
    $current = readline_info('pending_input');
    $info = readline_info();
    printf("old=%d current=%d info=%d\n",
        $old,
        $current,
        $info['pending_input']
    );
}
?>
--EXPECT--
old=0 current=127 info=127
old=127 current=128 info=128
old=128 current=254 info=254
old=254 current=255 info=255
old=255 current=0 info=0
