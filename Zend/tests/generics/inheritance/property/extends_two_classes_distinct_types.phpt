--TEST--
Property substitution: two classes extending the same generic parent get distinct property types
--FILE--
<?php
class Box<T> {
    public T $val;
}

class IntBox extends Box<int> {}
class StrBox extends Box<string> {}

$rcInt = (new ReflectionClass('IntBox'))->getProperty('val');
$rcStr = (new ReflectionClass('StrBox'))->getProperty('val');
echo $rcInt->getType()->getName(), "/", $rcStr->getType()->getName(), "\n";

$ib = new IntBox; $ib->val = 1;
$sb = new StrBox; $sb->val = "x";

try { $ib->val = "no"; } catch (TypeError) { echo "TypeError IntBox\n"; }
try { $sb->val = []; } catch (TypeError) { echo "TypeError StrBox\n"; }
?>
--EXPECT--
int/string
TypeError IntBox
TypeError StrBox
