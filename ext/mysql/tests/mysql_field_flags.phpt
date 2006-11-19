--TEST--
mysql_field_flags()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
    include "connect.inc";

    $tmp    = NULL;   
    $link   = NULL;    
    
    if (!is_null($tmp = @mysql_field_flags()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
        
    if (null !== ($tmp = @mysql_field_flags($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

    require('table.inc');
    if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 2", $link)) {
        printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
    }

    if (NULL !== ($tmp = mysql_field_flags($res)))
        printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
        
    if (false !== ($tmp = mysql_field_flags($res, -1)))
        printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
    
    if (!is_string($tmp = mysql_field_flags($res, 0)) || empty($tmp))
        printf("[006] Expecting non empty string, got %s/%s\n", gettype($tmp), $tmp);
                
    if (ini_get('unicode.semantics') && !is_unicode($tmp)) {
        printf("[007] Check the unicode support!\n");            
        var_inspect($tmp);        
    }
        
    if (false !== ($tmp = mysql_field_flags($res, 2)))
        printf("[008] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);   
    
    mysql_free_result($res);
    
    var_dump(mysql_field_flags($res, 0));
    
    mysql_close($link);    
    print "done!";
?>
--EXPECTF--
Warning: Wrong parameter count for mysql_field_flags() in %s on line %d

Warning: mysql_field_flags(): Field -1 is invalid for MySQL result index %d in %s on line %d

Warning: mysql_field_flags(): Field 2 is invalid for MySQL result index %d in %s on line %d

Warning: mysql_field_flags(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!
