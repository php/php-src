--TEST--
Namespace const declarations are inlined by the optimizer
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php
namespace MyNS;

const NS_CONST = 100;

function use_ns_const() {
    return NS_CONST;
}

function use_ns_const_fqn() {
    return \MyNS\NS_CONST;
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 DECLARE_CONST string("MyNS\\NS_CONST") int(100)
0001 RETURN int(1)

MyNS\use_ns_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(100)

MyNS\use_ns_const_fqn:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(100)
