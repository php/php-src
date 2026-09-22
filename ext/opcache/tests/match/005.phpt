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
     ; (lines=20, args=0, vars=2, tmps=1)
     ; (after optimizer)
     ; %s
0000 ASSIGN CV0($text) string("Bienvenue chez nous")
0001 T2 = FRAMELESS_ICALL_2(preg_match) string("/Welcome/") CV0($text)
0002 JMPNZ T2 0010
0003 T2 = FRAMELESS_ICALL_2(preg_match) string("/Hello/") CV0($text)
0004 JMPNZ T2 0010
0005 T2 = FRAMELESS_ICALL_2(preg_match) string("/Bienvenue/") CV0($text)
0006 JMPNZ T2 0012
0007 T2 = FRAMELESS_ICALL_2(preg_match) string("/Bonjour/") CV0($text)
0008 JMPNZ T2 0012
0009 JMP 0014
0010 T2 = QM_ASSIGN string("en")
0011 JMP 0015
0012 T2 = QM_ASSIGN string("fr")
0013 JMP 0015
0014 T2 = QM_ASSIGN string("other")
0015 ASSIGN CV1($result) T2
0016 INIT_FCALL 1 %d string("var_dump")
0017 SEND_VAR CV1($result) 1
0018 DO_ICALL
0019 RETURN int(1)
string(2) "fr"
