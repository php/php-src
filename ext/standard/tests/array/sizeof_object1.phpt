--TEST--
Test sizeof() function : object functionality - object with Countable interface
--FILE--
<?php
echo "*** Testing sizeof() : object functionality ***\n";

echo "-- Testing sizeof() with an object which implements Countable interface --\n";
class sizeof_class implements Countable
{
  public $member1;
  private $member2;
  protected $member3;

  public function count()
  {
    return 3; // return the count of member variables in the object
  }
}

$obj = new sizeof_class();

echo "-- Testing sizeof() in default mode --\n";
var_dump( sizeof($obj) );
echo "-- Testing sizeof() in COUNT_NORMAL mode --\n";
var_dump( sizeof($obj, COUNT_NORMAL) );
echo "-- Testing sizeof() in COUNT_RECURSIVE mode --\n";
var_dump( sizeof($obj, COUNT_RECURSIVE) );

echo "Done";
?>
--EXPECT--
*** Testing sizeof() : object functionality ***
-- Testing sizeof() with an object which implements Countable interface --
-- Testing sizeof() in default mode --
int(3)
-- Testing sizeof() in COUNT_NORMAL mode --
int(3)
-- Testing sizeof() in COUNT_RECURSIVE mode --
int(3)
Done
