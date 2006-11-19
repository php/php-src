--TEST--
mysql_unbuffered_query()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
    include "connect.inc";
    
    $tmp    = NULL;   
    $link   = NULL;    
    
    if (false !== ($tmp = @mysql_unbuffered_query($link)))
        printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
        
    require('table.inc');
    
    if (NULL !== ($tmp = @mysql_unbuffered_query("SELECT 1 AS a", $link, "foo")))
        printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);
    
    if (false !== ($tmp = mysql_unbuffered_query('THIS IS NOT SQL', $link)))
        printf("[003] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
        
    if (false !== ($tmp = mysql_unbuffered_query('SELECT "this is sql but with backslash g"\g', $link)))
        printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);        
        
    if ((0 === mysql_errno($link)) || ('' == mysql_error($link)))
        printf("[005] mysql_errno()/mysql_error should return some error\n");
        
    if (!$res = mysql_unbuffered_query('SELECT "this is sql but with semicolon" AS valid ; ', $link))
        printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));
    
    var_dump(mysql_fetch_assoc($res));    
    mysql_free_result($res);
        
    if (false !== ($res = mysql_unbuffered_query('SELECT "this is sql but with semicolon" AS valid ; SHOW VARIABLES', $link)))
        printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));
        
    if (mysql_unbuffered_query('DROP PROCEDURE IF EXISTS p', $link)) {
        // let's try to play with stored procedures        
        if (mysql_unbuffered_query('CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;', $link)) {
            $res = mysql_unbuffered_query('CALL p(@version)', $link);    
            $res = mysql_unbuffered_query('SELECT @version AS p_version', $link);
            var_dump(mysql_fetch_assoc($res));            
            mysql_free_result($res);
        } else {
            printf("[008] [%d] %s\n", mysql_errno($link), mysql_error($link));
        }
        
        mysql_unbuffered_query('DROP FUNCTION IF EXISTS f', $link);
        if (mysql_unbuffered_query('CREATE FUNCTION f( ver_param VARCHAR(25)) RETURNS VARCHAR(25) DETERMINISTIC RETURN ver_param;', $link)) {
            $res = mysql_unbuffered_query('SELECT f(VERSION()) AS f_version', $link);
            var_dump(mysql_fetch_assoc($res));
            mysql_free_result($res);
        } else {
            printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));
        }
    }
    
    var_dump(mysql_unbuffered_query('INSERT INTO test(id) VALUES (100)', $link));
    var_dump($res = mysql_unbuffered_query('SELECT id FROM test', $link));
    var_dump(mysql_num_rows($res));   
    
    mysql_close($link);
    
    if (false !== ($tmp = mysql_unbuffered_query("SELECT id FROM test", $link)))
        printf("[010] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);
    
    print "done!";
?>
--EXPECTF--
array(1) {
  ["valid"]=>
  string(30) "this is sql but with semicolon"
}
array(1) {
  ["p_version"]=>
  string(16) "5.0.26-debug-log"
}
array(1) {
  ["f_version"]=>
  string(16) "5.0.26-debug-log"
}
bool(true)
resource(%d) of type (mysql result)
int(0)

Notice: mysql_close(): Function called without first fetching all rows from a previous unbuffered query in %s on line %d

Warning: mysql_unbuffered_query(): %d is not a valid MySQL-Link resource in %s on line %d
done!
--UEXPECTF--
array(1) {
  [u"valid"]=>
  unicode(30) "this is sql but with semicolon"
}
array(1) {
  [u"p_version"]=>
  unicode(16) "5.0.26-debug-log"
}
array(1) {
  [u"f_version"]=>
  unicode(16) "5.0.26-debug-log"
}
bool(true)
resource(%d) of type (mysql result)
int(0)

Notice: mysql_close(): Function called without first fetching all rows from a previous unbuffered query in %s on line %d

Warning: mysql_unbuffered_query(): %d is not a valid MySQL-Link resource in %s on line %d
done!
