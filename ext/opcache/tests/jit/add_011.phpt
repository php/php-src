--TEST--
JIT ADD: 011 overflow handling
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
function test() {
    $n = $a = 0;
    while ($a <= 0) {
        $a &= $a-- + $a + $u;
        var_dump($a);
        if (++$n > 59) die("bug\n");
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-4)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-13)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-32)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-97)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-228)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-493)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1024)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-3073)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-7172)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-15373)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-31776)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-64609)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-130276)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-261613)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-524288)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1572865)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-3670020)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-7864333)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-16252960)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-33030241)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-66584804)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-133693933)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-267912192)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-536349697)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1073224708)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-2146974733)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-4294474784)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-8589474913)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-17179475172)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-34359475693)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-68719476736)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-206158430209)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-481036337156)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1030792151053)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-2130303778848)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-4329327034465)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-8727373545700)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-17523466568173)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-35115652613120)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-70300024704001)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-140668768885764)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-281406257249293)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-562881233976352)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1125831187430497)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-2251731094338788)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-4503530908155373)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-9007130535788544)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-18014329791578113)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-36028728303157252)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-72057525326315533)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-144115119372632096)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-288230307465265249)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-576460683650531556)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-1152921436021064173)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-2305842940762129408)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-4611685950244260865)

Warning: Undefined variable $u in %sadd_011.php on line 5
int(-9223371969208523780)

Warning: Undefined variable $u in %sadd_011.php on line 5

Deprecated: Implicit conversion from float %f to int loses precision in %sadd_011.php on line 5
int(66572500992)