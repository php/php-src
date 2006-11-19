--TEST--
mysql_free_result()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
    include "connect.inc";

    $tmp    = NULL;   
    $link   = NULL;    
    
    if (!is_null($tmp = @mysql_free_result()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
        
    if (false !== ($tmp = @mysql_free_result($link)))
        printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');    
    if (!$res = mysql_query("SELECT id FROM test ORDER BY id LIMIT 1", $link)) {
        printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
    } 
        
    var_dump(mysql_free_result($res));    
    var_dump(mysql_free_result($res));
        
    mysql_close($link);    
    print "done!";
?>
--EXPECTF--
bool(true)

Warning: mysql_free_result(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!