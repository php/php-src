--TEST--
mysql_result()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
    include "connect.inc";
    
    $tmp    = NULL;   
    $link   = NULL;    
    
    // string mysql_result ( resource result, int row [, mixed field] )
    
    if (!is_null($tmp = @mysql_result()))
        printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
        
    if (!is_null($tmp = @mysql_result($link)))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
        
    require('table.inc');
    if (!$res = mysql_query("SELECT id, label FROM test ORDER BY id LIMIT 1", $link)) {
        printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));        
    }
    
    var_dump(mysql_result($res, -1));
    var_dump(mysql_result($res, 2));
    var_dump(mysql_result($res, 0, -1));
    var_dump(mysql_result($res, 0, 2));
          
    var_dump(mysql_result($res, 0));
    var_dump(mysql_result($res, 0, 1));
    
    mysql_free_result($res);
    
    var_dump(mysql_result($res, 0));
    
    mysql_close($link);   
    print "done!";
?>
--EXPECTF--
Warning: mysql_result(): Unable to jump to row -1 on MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): Unable to jump to row 2 on MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): Bad column offset specified in %s on line %d
bool(false)

Warning: mysql_result(): Bad column offset specified in %s on line %d
bool(false)
string(1) "1"
string(1) "a"

Warning: mysql_result(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!
--UEXPECTF--
Warning: mysql_result(): Unable to jump to row -1 on MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): Unable to jump to row 2 on MySQL result index %d in %s on line %d
bool(false)

Warning: mysql_result(): Bad column offset specified in %s on line %d
bool(false)

Warning: mysql_result(): Bad column offset specified in %s on line %d
bool(false)
unicode(1) "1"
unicode(1) "a"

Warning: mysql_result(): %d is not a valid MySQL result resource in %s on line %d
bool(false)
done!