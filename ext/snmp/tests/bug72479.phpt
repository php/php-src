--TEST--
Bug #72479: Use After Free Vulnerability in SNMP with GC and unserialize()
--EXTENSIONS--
snmp
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
$arr = [1, [1, 2, 3, 4, 5], 3, 4, 5];
$poc = 'a:3:{i:1;N;i:2;O:4:"snmp":1:{s:11:"quick_print";'.serialize($arr).'}i:1;R:7;}';
$out = unserialize($poc);
gc_collect_cycles();
$fakezval = ptr2str(1122334455);
$fakezval .= ptr2str(0);
$fakezval .= "\x00\x00\x00\x00";
$fakezval .= "\x01";
$fakezval .= "\x00";
$fakezval .= "\x00\x00";
for ($i = 0; $i < 5; $i++) {
    $v[$i] = $fakezval.$i;
}
var_dump($out[1]);

function ptr2str($ptr)
{
    $out = '';
    for ($i = 0; $i < 8; $i++) {
        $out .= chr($ptr & 0xff);
        $ptr >>= 8;
    }
    return $out;
}
?>
--EXPECT--
int(1)
