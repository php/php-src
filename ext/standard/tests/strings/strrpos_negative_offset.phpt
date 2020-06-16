--TEST--
strr[i]pos() function with negative offset
--FILE--
<?php
    var_dump(strrpos("haysthack", 'ha', -9));
    var_dump(strrpos("haystack", 'h', -8));
    var_dump(strrpos("haystack", 'k', -1));
    var_dump(strrpos("haystack", "ka", -1));
    var_dump(strrpos("haystack", 'a', -3));
    var_dump(strrpos("haystack", 'a', -4));
    try {
        strrpos("haystack", 'h', -9);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
    var_dump(strripos("HAYSTHACk", 'ha', -9));
    var_dump(strripos("HAYSTACK", 'h', -8));
    var_dump(strripos("HAYSTACK", 'k', -1));
    var_dump(strripos("HAYSTACK", "ka", -1));
    var_dump(strripos("HAYSTACK", 'a', -3));
    var_dump(strripos("HAYSTACK", 'a', -4));
    try {
        strripos("HAYSTACK", 'h', -9);
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
?>
--EXPECT--
int(0)
int(0)
int(7)
bool(false)
int(5)
int(1)
strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
int(0)
int(0)
int(7)
bool(false)
int(5)
int(1)
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
