--TEST--
Test posix_ctermid()
--DESCRIPTION--
Gets path name of controlling terminal.
Source code: ext/posix/posix.c
--CREDITS--
Falko Menge, mail at falko-menge dot de
PHP Testfest Berlin 2009-05-10
--EXTENSIONS--
posix
--SKIPIF--
<?php
    
    // needed because of #ifdef HAVE_CTERMID in posix.c
    if (!function_exists('posix_ctermid')) {
        die('SKIP - Function posix_ctermid() not available');
    }
?>
--FILE--
<?php
    var_dump(posix_ctermid());
?>
--EXPECTF--
string(%d) "%s"
