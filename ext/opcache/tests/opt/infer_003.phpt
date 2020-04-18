--TEST--
Infer 003: '==='/'!==' optimizations
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class X {
    public static function create_string(int $i): string {
        return "a$i";
    }
    public static function foo(int $a) {
        for ($i = 0; $i < 2; $i++) {
            if (self::create_string($i) === $a) {
                echo "Impossible\n";
            }
            if (self::create_string($i) === []) {
                echo "Impossible\n";
            }
            if (self::create_string($i) === self::create_string(1)) {
                echo "Success for $i\n";
            }
        }
    }
}
X::foo(1);
?>
--EXPECTF--
$_main:
     ; (lines=4, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s:1-22
0000 INIT_STATIC_METHOD_CALL 1 string("X") string("foo")
0001 SEND_VAL int(1) 1
0002 DO_UCALL
0003 RETURN int(1)

X::create_string:
     ; (lines=3, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s:3-5
0000 CV0($i) = RECV 1
0001 T1 = FAST_CONCAT string("a") CV0($i)
0002 RETURN T1

X::foo:
     ; (lines=29, args=1, vars=2, tmps=3)
     ; (after optimizer)
     ; %s:6-18
0000 CV0($a) = RECV 1
0001 CV1($i) = QM_ASSIGN int(0)
0002 JMP 0026
0003 INIT_STATIC_METHOD_CALL 1 (self) (exception) string("create_string")
0004 SEND_VAR CV1($i) 1
0005 DO_UCALL
0006 JMP 0008
0007 ECHO string("Impossible
")
0008 INIT_STATIC_METHOD_CALL 1 (self) (exception) string("create_string")
0009 SEND_VAR CV1($i) 1
0010 DO_UCALL
0011 JMP 0013
0012 ECHO string("Impossible
")
0013 INIT_STATIC_METHOD_CALL 1 (self) (exception) string("create_string")
0014 SEND_VAR CV1($i) 1
0015 V3 = DO_UCALL
0016 INIT_STATIC_METHOD_CALL 1 (self) (exception) string("create_string")
0017 SEND_VAL int(1) 1
0018 V4 = DO_UCALL
0019 T2 = IS_IDENTICAL V3 V4
0020 JMPZ T2 0025
0021 T3 = ROPE_INIT 3 string("Success for ")
0022 T3 = ROPE_ADD 1 T3 CV1($i)
0023 T2 = ROPE_END 2 T3 string("
")
0024 ECHO T2
0025 PRE_INC CV1($i)
0026 T2 = IS_SMALLER CV1($i) int(2)
0027 JMPNZ T2 0003
0028 RETURN null
LIVE RANGES:
     3: 0016 - 0019 (tmp/var)
     3: 0021 - 0023 (rope)
Success for 1