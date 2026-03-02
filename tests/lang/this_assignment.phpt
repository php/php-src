--TEST--
Test to catch early assignment of $this
--FILE--
<?php
class first {

   function me() { echo "first"; }

   function who() {
     global $a,$b;
     $this->me();
     $a->me();
     $b->me();
     $b = new second();
     $this->me();
     $a->me();
     $b->me();
   }
}

class second {

   function who() {
      global $a,$b;
      $this->me();
      $a->me();
      $b->me();
   }
   function me() { echo "second"; }
}

$a = new first();
$b = &$a;

$a->who();
$b->who();

echo "\n";
?>
--EXPECT--
firstfirstfirstfirstsecondsecondsecondsecondsecond
