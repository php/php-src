--TEST--
mysql_stat()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    include_once "connect.inc";

    $dbname = 'test';    
    $tmp    = NULL;   
    $link   = NULL;    
    
    if (!is_null($tmp = @mysql_stat($link)))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');
    
    if (!is_null($tmp = @mysql_stat($link, "foo")))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
    
    if ((!is_string($tmp = mysql_stat($link))) || ('' === $tmp))
        printf("[003] Expecting non empty string, got %s/'%s', [%d] %s\n", 
            gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));
            
    if (ini_get('unicode.semantics') && !is_unicode($tmp)) {
        printf("[004] Expecting Unicode error message!\n");
        var_inspect($tmp);
    }    
    
    mysql_close($link); 
    
    if (false !== ($tmp = mysql_stat($link)))
        printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
           
    print "done!";
?>
--EXPECTF--
Warning: mysql_stat(): %d is not a valid MySQL-Link resource in %s on line %d
done!
