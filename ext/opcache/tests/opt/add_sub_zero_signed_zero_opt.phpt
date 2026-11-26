--TEST--
DFA pass must keep a float + 0.0 that can turn -0.0 into 0.0, and still drop the real identities
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php
/* x + 0.0 can turn -0.0 into 0.0, so these keep the ADD/SUB */
function add_int_zero(float $x) { return $x + 0; }
function add_zero_left(float $x) { return 0 + $x; }
function add_float_zero(float $x) { return $x + 0.0; }
function add_float_zero_left(float $x) { return 0.0 + $x; }
function sub_neg_zero(float $x) { return $x - -0.0; }

/* These give back x for every float and still become a copy */
function sub_zero(float $x) { return $x - 0.0; }
function sub_int_zero(float $x) { return $x - 0; }
function add_neg_zero(float $x) { return $x + -0.0; }
function add_neg_zero_left(float $x) { return -0.0 + $x; }
function int_add_zero(int $x) { return $x + 0; }
?>
--EXPECTF--

$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:1-16
0000 RETURN int(1)

add_int_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:3-3
0000 CV0($x) = RECV 1
0001 T1 = ADD CV0($x) float(0)
0002 RETURN T1

add_zero_left:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:4-4
0000 CV0($x) = RECV 1
0001 T1 = ADD CV0($x) float(0)
0002 RETURN T1

add_float_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:5-5
0000 CV0($x) = RECV 1
0001 T1 = ADD CV0($x) float(0)
0002 RETURN T1

add_float_zero_left:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:6-6
0000 CV0($x) = RECV 1
0001 T1 = ADD CV0($x) float(0)
0002 RETURN T1

sub_neg_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:7-7
0000 CV0($x) = RECV 1
0001 T1 = SUB CV0($x) float(-0)
0002 RETURN T1

sub_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:10-10
0000 CV0($x) = RECV 1
0001 T1 = QM_ASSIGN CV0($x)
0002 RETURN T1

sub_int_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:11-11
0000 CV0($x) = RECV 1
0001 T1 = QM_ASSIGN CV0($x)
0002 RETURN T1

add_neg_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:12-12
0000 CV0($x) = RECV 1
0001 T1 = QM_ASSIGN CV0($x)
0002 RETURN T1

add_neg_zero_left:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:13-13
0000 CV0($x) = RECV 1
0001 T1 = QM_ASSIGN CV0($x)
0002 RETURN T1

int_add_zero:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %sadd_sub_zero_signed_zero_opt.php:14-14
0000 CV0($x) = RECV 1
0001 T1 = QM_ASSIGN CV0($x)
0002 RETURN T1
