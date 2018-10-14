--TEST--
Check xsltprocessor::getParameter with undefined parameter
--SKIPIF--
<?php
        if (!extension_loaded('xsl')) {
                die("skip\n");
        }
?>
--FILE--
<?php
include dirname(__FILE__) .'/prepare.inc';
var_dump($proc->getParameter('', 'doesnotexist'));
--EXPECTF--
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
