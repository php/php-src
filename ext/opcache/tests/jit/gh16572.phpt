--TEST--
GH-16572 (Incorrect result with reflection in low-trigger JIT)
--EXTENSIONS--
opcache
--INI--
opcache.jit=1215
--FILE--
<?php
function dumpType(ReflectionType $rt) {
    var_dump($rt::class);
    dumpType(null);
}
function test1(): int { }
dumpType((new ReflectionFunction('test1'))->getReturnType());
?>
--EXPECTF--
string(19) "ReflectionNamedType"

Fatal error: Uncaught TypeError: dumpType(): Argument #1 ($rt) must be of type ReflectionType, null given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): dumpType(NULL)
#1 %s(%d): dumpType(Object(ReflectionNamedType))
#2 {main}
  thrown in %s on line %d
