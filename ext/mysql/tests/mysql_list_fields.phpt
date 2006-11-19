--TEST--
mysql_list_fields()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    include_once "connect.inc";

    $tmp    = NULL;   
    $link   = NULL;
       
    if (false !== ($tmp = @mysql_list_fields($link, $link)))
        printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);     
            
    require('table.inc');
       
    if (!$res = mysql_list_fields($db, 'test', $link))
        printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));
     
    if (!($num = mysql_num_fields($res)))
        printf("[004] Empty field list? [%d] %s\n", mysql_errno($link), mysql_error($link));

        
    mysql_free_result($res);    
    mysql_close($link);
        
    print "done!\n"; 
?>
--EXPECTF--
done!
