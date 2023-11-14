--TEST--
JIT Shift Right: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function encodeDynamicInteger(int $int): string {
    $out = "";
    for ($i = 0; ($int >> $i) > 0x80; $i += 7) {
        $out .= \chr(0x80 | (($int >> $i) & 0x7f));
    }
    return $out . \chr($int >> $i);
}
$s = encodeDynamicInteger(235);
var_dump(strlen($s), ord($s[0]), ord($s[1]));
?>
--EXPECT--
int(2)
int(235)
int(1)
