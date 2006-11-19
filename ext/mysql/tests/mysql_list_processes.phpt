--TEST--
mysql_list_processes()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
    include "connect.inc";

    $tmp    = NULL;   
    $link   = NULL;
       
    if (NULL !== ($tmp = @mysql_list_processes($link, $link)))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);     
            
    require('table.inc');
       
    if (!$res = mysql_list_processes($link))
        printf("[002] [%d] %s\n", mysql_errno($link), mysql_error($link));
     
    if (!$num = mysql_num_rows($res))
        printf("[003] Empty process list? [%d] %s\n", mysql_errno($link), mysql_error($link));
        
    $row = mysql_fetch_array($res, MYSQL_NUM);
    if (ini_get('unicode.semantics') && !is_unicode($row[0])) {
        printf("[004] Check for unicode support\n");
        var_inspect($row);
    }
            
    mysql_free_result($res);    
    mysql_close($link);
        
    print "done!\n"; 
?>
--EXPECTF--
done!
