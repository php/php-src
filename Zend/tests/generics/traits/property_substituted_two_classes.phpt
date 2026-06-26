--TEST--
Traits: two classes using the same generic trait with different bindings get distinct property types
--FILE--
<?php
trait Holder<T> {
    public T $val;
}

class IntBox { use Holder<int>; }
class StrBox { use Holder<string>; }

$ib = new IntBox;
$ib->val = 1;
$sb = new StrBox;
$sb->val = "x";

$rcInt = (new ReflectionClass('IntBox'))->getProperty('val');
$rcStr = (new ReflectionClass('StrBox'))->getProperty('val');
echo $rcInt->getType()->getName(), "/", $rcStr->getType()->getName(), "\n";

try { $ib->val = "no"; } catch (TypeError) { echo "TypeError IntBox\n"; }
try { $sb->val = []; } catch (TypeError) { echo "TypeError StrBox\n"; }
?>
--EXPECT--
int/string
TypeError IntBox
TypeError StrBox
