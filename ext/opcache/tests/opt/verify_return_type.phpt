--TEST--
Return type check elision
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.opt_debug_level=0x20000
opcache.preload=
--EXTENSIONS--
opcache
--FILE--
<?php

class Test1 {
    final public function getIntOrFloat(int $i): int|float {
        return $i;
    }
    final public function getInt(): int {
        return $this->getIntOrFloat();
    }
}

class Test2 {
    public function getInt(): int {
        return 42;
    }
    public function getInt2(): int {
        return $this->getInt();
    }
    public function getIntOrFloat(int $i): int|float {
        return $i;
    }
    public function getInt3(int $i): int {
        // Should not elide return type check. Test2::getIntOrFloat() returns only int,
        // but a child method may return int|float.
        return $this->getIntOrFloat($i);
    }
}

class Test3 {
    private function getBool() {
        return true;
    }

    private function getBool2(): bool {
        return $this->getBool();
    }
}

function getClassUnion(): stdClass|FooBar {
    return new stdClass;
}

function getClassIntersection(): Traversable&Countable {
    return new ArrayObject;
}

function test4(Test1 $t1): int {
    return $t1->getInt();
}

function test5(Test1 $t1, Test2 $t2): int {
    $r = $t1->getInt();
    if (rand()) {
        $r = $t2->getInt();
    }
    return $r;
}
?>
--EXPECTF--
$_main:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(1)

getClassUnion:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 V0 = NEW 0 string("stdClass")
0001 DO_FCALL
0002 RETURN V0
LIVE RANGES:
     0: 0001 - 0002 (new)

getClassIntersection:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 V0 = NEW 0 string("ArrayObject")
0001 DO_FCALL
0002 RETURN V0
LIVE RANGES:
     0: 0001 - 0002 (new)

test4:
     ; (lines=4, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; /home/arnaud/dev/php-src/ext/opcache/tests/opt/verify_return_type.php:47-49
0000 CV0($t1) = RECV 1
0001 INIT_METHOD_CALL 0 CV0($t1) string("getInt")
0002 V1 = DO_FCALL
0003 RETURN V1

test5:
     ; (lines=12, args=2, vars=3, tmps=1)
     ; (after optimizer)
     ; /home/arnaud/dev/php-src/ext/opcache/tests/opt/verify_return_type.php:51-57
0000 CV0($t1) = RECV 1
0001 CV1($t2) = RECV 2
0002 INIT_METHOD_CALL 0 CV0($t1) string("getInt")
0003 CV2($r) = DO_FCALL
0004 INIT_FCALL 0 80 string("rand")
0005 V3 = DO_ICALL
0006 JMPZ V3 0010
0007 INIT_METHOD_CALL 0 CV1($t2) string("getInt")
0008 V3 = DO_FCALL
0009 CV2($r) = QM_ASSIGN V3
0010 VERIFY_RETURN_TYPE CV2($r)
0011 RETURN CV2($r)

Test1::getIntOrFloat:
     ; (lines=2, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s
0000 CV0($i) = RECV 1
0001 RETURN CV0($i)

Test1::getInt:
     ; (lines=3, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 INIT_METHOD_CALL 0 THIS string("getIntOrFloat")
0001 V0 = DO_UCALL
0002 RETURN V0

Test2::getInt:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

Test2::getInt2:
     ; (lines=4, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 INIT_METHOD_CALL 0 THIS string("getInt")
0001 V0 = DO_FCALL
0002 VERIFY_RETURN_TYPE V0
0003 RETURN V0
LIVE RANGES:
     0: 0002 - 0003 (tmp/var)

Test2::getIntOrFloat:
     ; (lines=2, args=1, vars=1, tmps=0)
     ; (after optimizer)
     ; %s
0000 CV0($i) = RECV 1
0001 RETURN CV0($i)

Test2::getInt3:
     ; (lines=6, args=1, vars=1, tmps=1)
     ; (after optimizer)
     ; %s
0000 CV0($i) = RECV 1
0001 INIT_METHOD_CALL 1 THIS string("getIntOrFloat")
0002 SEND_VAR CV0($i) 1
0003 V1 = DO_FCALL
0004 VERIFY_RETURN_TYPE V1
0005 RETURN V1
LIVE RANGES:
     1: 0004 - 0005 (tmp/var)

Test3::getBool:
     ; (lines=1, args=0, vars=0, tmps=0)
     ; (after optimizer)
     ; %s
0000 RETURN bool(true)

Test3::getBool2:
     ; (lines=2, args=0, vars=0, tmps=1)
     ; (after optimizer)
     ; %s
0000 V0 = QM_ASSIGN bool(true)
0001 RETURN V0
