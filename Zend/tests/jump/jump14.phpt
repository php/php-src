--TEST--
Testing GOTO inside blocks
--FILE--
<?php

goto A;

{
    B:
        goto C;
        return;
}

A:
    goto B;



{
    C:
    {
        print "Done!\n";
    }
}

?>
--EXPECT--
Done!
