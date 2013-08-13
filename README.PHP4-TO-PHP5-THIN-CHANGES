1. strrpos() and strripos() now use the entire string as a needle.  Be aware
   that the existing scripts may no longer work as you expect.

   EX :
   <?php
   var_dump(strrpos("ABCDEF","DEF"));
   var_dump(strrpos("ABCDEF","DAF"));
   ?>

   Will give you different results. The former returns 3 while the latter
   returns false rather than the position of the last occurrence of 'D'.
   The same applies to strripos().

2. Illegal use of string offsets causes E_ERROR instead of E_WARNING.

   EX :
   <?php
   $a = "foo";
   unset($a[0][1][2]);
   ?>

   Fatal error: Cannot use string offset as an array in ... on line 1

3. array_merge() was changed to accept only arrays. If a non-array variable is
   passed, a E_WARNING will be thrown for every such parameter. Be careful
   because your code may start emitting E_WARNING out of the blue.

4. Be careful when porting from ext/mysql to ext/mysqli. The following
   functions return NULL when no more data is available in the result set
   (ext/mysql's functions return FALSE).

    - mysqli_fetch_row()
    - mysqli_fetch_array()
    - mysqli_fetch_assoc()

5. PATH_TRANSLATED server variable is no longer set implicitly under
   Apache2 SAPI in contrast to the situation in PHP 4, where it is set to the
   same value as the SCRIPT_FILENAME server variable when it is not populated
   by Apache.  This change was made to comply with the CGI specification.
   Please refer to bug #23610 for further information.

6. Starting PHP 5.0.0 the T_ML_CONSTANT constant is no longer defined by the
   ext/tokenizer extension. If error_reporting is set to E_ALL notices will
   be produced. Instead of T_ML_CONSTANT for /* */ the T_COMMENT constant 
   is used, thus both // and /* */ are resolved as the T_COMMENT constant.
   However the PHPDoc style comments /** */ ,which starting PHP 5 are parsed
   by PHP, are recongnized as T_DOC_COMMENT.

7. $_SERVER should be populated with argc and argv if variables_order
   includes "S".  If you have specifically configured your system to not
   create $_SERVER, then of course it shouldn't be there.  The change was to
   always make argc and argv available in the CLI version regardless of the
   variables_order setting.  As in, the CLI version will now always populate
   the global $argc and $argv variables.

8. In some cases classes must be declared before used. It only happens only
   if some of the new features of PHP 5 are used. Otherwise the behaviour is
   the old.
   Example 1 (works with no errors):
   <?php
   $a = new a();
   class a {
   }
   ?>
 
   Example 2 (throws an error):
   <?php 
   $a = new a();
   interface b{
   }
   class a implements b {
   } 
   ?>

   Output (example 2) :
   Fatal error: Class 'a' not found in /tmp/cl.php on line 2

9. get_class() starting PHP 5 returns the name of the class as it was
   declared which may lead to problems in older scripts that rely on
   the previous behaviour - the class name is lowercased. Expect the
   same behaviour from get_parent_class() when applicable.
   Example :
   <?php
   class FooBar {
   }
   class ExtFooBar extends FooBar{}
   $a = new FooBar();
   var_dump(get_class($a), get_parent_class($a));
   ?>

   Output (PHP 4):
   string(6) "foobar"
   string(9) "extfoobar"

   Output (PHP 5):
   string(6) "FooBar"
   string(9) "ExtFooBar"
   ----------------------------------------------------------------------
   Example code that will break :
   //....
   function someMethod($p) {
     if (get_class($p) != 'helpingclass') {
       return FALSE;
     }
     //...
   }
   //...
   Possible solution is to search for get_class() and get_parent_class() in
   all your scripts and use strtolower().

10. get_class_methods() returns the names of the methods of a class as they
   declared. In PHP4 the names are all lowercased.
   Example code :
   <?php
   class Foo{
     function doFoo(){}
     function hasFoo(){}
   }
   var_dump(get_class_methods("Foo")); 
   ?>
   Output (PHP4):
   array(2) {
     [0]=>
     string(5) "dofoo"
     [1]=>
     string(6) "hasfoo"
   }
   Output (PHP5):
   array(2) {
     [0]=>
     string(5) "doFoo"
     [1]=>
     string(6) "hasFoo"
   }

11. Assignment $this is impossible. Starting PHP 5.0.0 $this has special
    meaning in class methods and is recognized by the PHP parser. The latter
    will generate a parse error when assignment to $this is found
    Example code :
    <?php
    class Foo {
      function assignNew($obj) {
        $this = $obj;
      }
    }
    $a = new Foo();
    $b = new Foo();
    $a->assignNew($b);
    echo "I was executed\n";
    ?>
    Output (PHP 4):
    I was executed
    Output (PHP 5):
    PHP Fatal error:  Cannot re-assign $this in /tmp/this_ex.php on line 4

