--TEST--
mysql_get_client_info()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
    include "connect.inc";
    if (!is_string($info = mysql_get_client_info()) || ('' === $info))
        printf("[001] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);        
        
    if (ini_get('unicode.semantics') && !is_unicode($info)) {
        printf("[002] Expecting Unicode!\n");
        var_inspect($info);
    }

    print "done!";
?>
--EXPECTF--
done!