--TEST--
Throwing an exception from an accessor
--FILE--
<?php

class Foo {
    public $bar {
        get { throw new Exception('except in getter'); }
        set { throw new Exception('except in setter'); }
        isset { throw new Exception('except in issetter'); }
        unset { throw new Exception('except in unsetter'); }
    }
}

$foo = new Foo;

try { $foo->bar; } catch (Exception $e) { echo $e, "\n\n"; }
try { $foo->bar = 'test'; } catch (Exception $e) { echo $e, "\n\n"; }
try { isset($foo->bar); } catch (Exception $e) { echo $e, "\n\n"; }
try { unset($foo->bar); } catch (Exception $e) { echo $e, "\n\n"; }

?>
--EXPECTF--
exception 'Exception' with message 'except in getter' in %s:%d
Stack trace:
#0 %s(%d): Foo->$bar->get()
#1 {main}

exception 'Exception' with message 'except in setter' in %s:%d
Stack trace:
#0 %s(%d): Foo->$bar->set('test')
#1 {main}

exception 'Exception' with message 'except in issetter' in %s:%d
Stack trace:
#0 %s(%d): Foo->$bar->isset()
#1 {main}

exception 'Exception' with message 'except in unsetter' in %s:%d
Stack trace:
#0 %s(%d): Foo->$bar->unset()
#1 {main}
