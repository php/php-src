--TEST--
Internal hash position bug on assignment (Bug #16227)
--POST--
--GET--
--FILE--
<?php
// reported by php.net@alienbill.com
       $outsidearray = array("key1","key2");
       $insidearray = array("0","1");

           while(list(,$outerval) = each($outsidearray)){
               $placeholder = $insidearray;
               while(list(,$innerval) = each($insidearray)){
                       print "inloop $innerval for $outerval\n";
               }
       }
?>
--EXPECT--
inloop 0 for key1
inloop 1 for key1

