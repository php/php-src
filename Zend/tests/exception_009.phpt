--TEST--
Testing exception properties
--FILE--
<?php

class my_file
{
         public  function __toString()
         {
                 return "somebuildfilename" ;
         }
}

class my_exception extends exception
{
         public  function __construct()
         {
                 $this->message = new stdclass ;
                 $this->file = new my_file ;
                 $this->line = "12" ;
         }
}

throw new my_exception;

?>
--EXPECT--
Fatal error: Uncaught Error: Object of class stdClass could not be converted to string in [no active file]:0
Stack trace:
#0 [internal function]: Exception->__toString()
#1 {main}
  thrown in [no active file] on line 0
