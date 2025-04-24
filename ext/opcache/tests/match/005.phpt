--TEST--
Match expression true
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
zend_test.observer.enabled=0
--EXTENSIONS--
opcache
--FILE--
<?php

$text = 'Bienvenue chez nous';

$result = match (true) {
    !!preg_match('/Welcome/', $text), !!preg_match('/Hello/', $text) => 'en',
    !!preg_match('/Bienvenue/', $text), !!preg_match('/Bonjour/', $text) => 'fr',
    default => 'other',
};

var_dump($result);

?>
--EXPECTF--
$_main:
     ; (lines=28, args=0, vars=2, tmps=3)
     ; (after optimizer)
     ; %s
0000 ASSIGN CV0($text) string("Bienvenue chez nous")
0001 T2 = FRAMELESS_ICALL_2(preg_match) string("/Welcome/") CV0($text)
0002 T3 = BOOL T2
0003 T2 = TYPE_CHECK (true) T3
0004 JMPNZ T2 0018
0005 T4 = FRAMELESS_ICALL_2(preg_match) string("/Hello/") CV0($text)
0006 T3 = BOOL T4
0007 T2 = TYPE_CHECK (true) T3
0008 JMPNZ T2 0018
0009 T4 = FRAMELESS_ICALL_2(preg_match) string("/Bienvenue/") CV0($text)
0010 T3 = BOOL T4
0011 T2 = TYPE_CHECK (true) T3
0012 JMPNZ T2 0020
0013 T4 = FRAMELESS_ICALL_2(preg_match) string("/Bonjour/") CV0($text)
0014 T3 = BOOL T4
0015 T2 = TYPE_CHECK (true) T3
0016 JMPNZ T2 0020
0017 JMP 0022
0018 T2 = QM_ASSIGN string("en")
0019 JMP 0023
0020 T2 = QM_ASSIGN string("fr")
0021 JMP 0023
0022 T2 = QM_ASSIGN string("other")
0023 ASSIGN CV1($result) T2
0024 INIT_FCALL 1 %d string("var_dump")
0025 SEND_VAR CV1($result) 1
0026 DO_ICALL
0027 RETURN int(1)
string(2) "fr"
