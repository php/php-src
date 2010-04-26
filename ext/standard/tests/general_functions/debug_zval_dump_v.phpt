--TEST--
Test debug_zval_dump() function : usage variations
--FILE--
<?php
/* Prototype: void debug_zval_dump ( mixed $variable );
   Description: Dumps a string representation of an internal zend value 
                to output.
*/

echo "*** Testing debug_zval_dump() on functions ***\n";
echo "--- Variation 1: global variable inside a function ---\n";
$global_var = 10;  //declaring global variable

/* function to dump reference count of global variable,$global_var
   and local variable,$local_var */
function dump_globalvar( &$local_var ) {
  global $global_var;
  echo "\n-- Value of local variable inside dump_globalvar() --\n";
  debug_zval_dump( $local_var );
  echo "\n-- Value of global variable inside dump_globalvar() --\n";
  debug_zval_dump( $global_var );
}
/* dump value and reference count of $global_var using debug_zval_dump() */
echo "\n-- Value of global variable, before calling dump_globalvar() --\n";
debug_zval_dump( $global_var );

/* calling function dump_globalvar() to check the reference count of local
   and global variables inside the function */
dump_globalvar( $global_var );

/* dump value and reference count of $global_var after exiting function
   dump_globalvar();
   expected: reference count of $global_var should remain the same as
             before calling dump_globalvar() function */
echo "\n-- Value of global variable, after exiting dump_globalvar() --\n";
debug_zval_dump( $global_var );

echo "\n--- Variation 2: one variable references another ---\n";
$first_var = 10;
/* dump value and reference count of $first_var */
echo "\n-- Value of \$first_var: --\n";
debug_zval_dump($first_var);

/* $ref_first_var references $first_var */
$ref_first_var = &$var_1;

echo "\n-- Value of \$ref_first_var --\n";
debug_zval_dump($ref_first_var);
echo "\n-- Value of \$first_var --\n";
debug_zval_dump($first_var);

unset($ref_first_var); 

/* dump value and reference count of $first_var, $ref_first_var
   here $ref_first_var is unset */
echo "\n-- Value of \$ref_first_var --\n";
debug_zval_dump($ref_first_var);
echo "\n-- Value of \$first_var --\n";
debug_zval_dump($first_var);

echo "\n--- Variation 3: multiple references of variables ---\n";
$var_1 = 10;
$var_2 = &$var_1;
$var_3 = &$var_2;
echo "\n-- Value of \$var_1: (before referencing) --\n";
debug_zval_dump($var_1);
echo "\n-- Value of \$var_2: (referencing var_1) --\n";
debug_zval_dump($var_2);
echo "\n-- Value of \$var_3: (referencing var_2) --\n";
debug_zval_dump($var_3);

/* unsetting $var_3 */
unset($var_3);
echo "\n-- Value of \$var_3: (after unsetting var_3) --\n"; 
debug_zval_dump($var_3);
echo "\n-- Value of \$var_2: --\n";
debug_zval_dump($var_2);
echo "\n-- Value of \$var_3: --\n";
debug_zval_dump($var_1);

/* unsetting $var_1 */
unset($var_1);
echo "\n-- Value of \$var_1: (after unsetting variable_1) --\n";
debug_zval_dump($var_1);
echo "\n-- Value of \$var_2: --\n";
debug_zval_dump($var_2);

echo "\n*** Testing debug_zval_dump() on miscelleneous input arguments ***\n";
/* unset a variable */
$unset_var = 10.5;
unset($unset_var);

$misc_values = array (
  /* nulls */
  NULL,
  null,
  
  /* unset variable */
  @$unset_var,

  /* undefined variable */
  @$undef_var,

 /* mixed types */
  @TRUE123,
  "123string",
  "string123",
  "NULLstring"
);
/* loop to display the variables and its reference count using
    debug_zval_dump() */
$counter = 1;
foreach( $misc_values as $value ) {
  echo "-- Iteration $counter --\n";
  debug_zval_dump( $value );
  $counter++;
}

echo "Done\n";
?>

--EXPECTF--
*** Testing debug_zval_dump() on functions ***
--- Variation 1: global variable inside a function ---

-- Value of global variable, before calling dump_globalvar() --
long(10) refcount(2)

-- Value of local variable inside dump_globalvar() --
long(10) refcount(1)

-- Value of global variable inside dump_globalvar() --
long(10) refcount(1)

-- Value of global variable, after exiting dump_globalvar() --
long(10) refcount(2)

--- Variation 2: one variable references another ---

-- Value of $first_var: --
long(10) refcount(2)

-- Value of $ref_first_var --
NULL refcount(1)

-- Value of $first_var --
long(10) refcount(2)

-- Value of $ref_first_var --

Notice: Undefined variable: ref_first_var in %s on line %d
NULL refcount(1)

-- Value of $first_var --
long(10) refcount(2)

--- Variation 3: multiple references of variables ---

-- Value of $var_1: (before referencing) --
long(10) refcount(1)

-- Value of $var_2: (referencing var_1) --
long(10) refcount(1)

-- Value of $var_3: (referencing var_2) --
long(10) refcount(1)

-- Value of $var_3: (after unsetting var_3) --

Notice: Undefined variable: var_3 in %s on line %d
NULL refcount(1)

-- Value of $var_2: --
long(10) refcount(1)

-- Value of $var_3: --
long(10) refcount(1)

-- Value of $var_1: (after unsetting variable_1) --

Notice: Undefined variable: var_1 in %s on line %d
NULL refcount(1)

-- Value of $var_2: --
long(10) refcount(2)

*** Testing debug_zval_dump() on miscelleneous input arguments ***
-- Iteration 1 --
NULL refcount(3)
-- Iteration 2 --
NULL refcount(3)
-- Iteration 3 --
NULL refcount(1)
-- Iteration 4 --
NULL refcount(1)
-- Iteration 5 --
string(7) "TRUE123" refcount(3)
-- Iteration 6 --
string(9) "123string" refcount(3)
-- Iteration 7 --
string(9) "string123" refcount(3)
-- Iteration 8 --
string(10) "NULLstring" refcount(3)
Done
