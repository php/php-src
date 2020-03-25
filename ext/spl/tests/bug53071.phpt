--TEST--
Bug #53071 (Usage of SPLObjectStorage defeats gc_collect_cycles)
--FILE--
<?php
gc_enable();
class myClass
{
    public $member;
}
function LimitedScope()
{
    $myA = new myClass();
    $myB = new SplObjectStorage();
    $myC = new myClass();
    $myC->member = $myA; // myC has a reference to myA
    $myB->Attach($myC);  // myB attaches myC
    $myA->member = $myB; // myA has myB, comleting the cycle
}
LimitedScope();
var_dump(gc_collect_cycles());

echo "Done.\n";

?>
--EXPECT--
int(3)
Done.
