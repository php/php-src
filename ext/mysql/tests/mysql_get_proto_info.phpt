--TEST--
mysql_get_proto_info()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
    include_once "connect.inc";
       
    if (false !== ($tmp = @mysql_get_proto_info(NULL)))
        printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);        
    
    require "table.inc";    
    if (!is_int($info = mysql_get_proto_info($link)) || (0 === $info))
        printf("[003] Expecting int/any_non_empty, got %s/%s\n", gettype($info), $info);        

    print "done!";
?>
--EXPECTF--
done!
