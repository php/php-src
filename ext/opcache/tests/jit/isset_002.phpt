--TEST--
ISSET_ISEMPTY_DIM with undefined variable
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo() {
    foreach ($how as $key => $value) {
        if (is_int($key)) {
            if (is_int($key)) {
                $keyy++;
            }
            if (isset($row[$key])) {
                return false;
            }
        }
        if (isset($row[$key])) {
            return false;
        }
    }
    return true;
}
foo();
?>
DONE
--EXPECTF--
Warning: Undefined variable $how in %sisset_002.php on line 3

Warning: foreach() argument must be of type array|object, null given in %sisset_002.php on line 3
DONE