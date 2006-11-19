--TEST--
mysql_thread_id()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    include_once "connect.inc";
        
    $tmp    = NULL;   
    $link   = NULL;    
    
    if (!is_null($tmp = @mysql_thread_id($link)))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);       

    require('table.inc');    
    
    if (!is_int($tmp = mysql_thread_id($link)) || (0 === $tmp))
        printf("[002] Expecting int/any but zero, got %s/%s. [%d] %s\n",
            gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));      
            
    mysql_close($link);
    
    if (false !== ($tmp = mysql_thread_id($link)))
        printf("[003] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
    
    print "done!";
?>
--EXPECTF--
Warning: mysql_thread_id(): %d is not a valid MySQL-Link resource in %s on line %d
done!
