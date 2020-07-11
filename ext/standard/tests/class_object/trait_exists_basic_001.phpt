--TEST--
Test trait_exists() function : basic functionality
--FILE--
<?php
echo "*** Testing trait_exists() : basic functionality ***\n";

spl_autoload_register(function ($traitName) {
    echo "In autoload($traitName)\n";
});

trait MyTrait {}

echo "Calling trait_exists() on non-existent trait with autoload explicitly enabled:\n";
var_dump( trait_exists('C', true) );
echo "\nCalling trait_exists() on existing trait with autoload explicitly enabled:\n";
var_dump( trait_exists('MyTrait', true) );

echo "\nCalling trait_exists() on non-existent trait with autoload explicitly enabled:\n";
var_dump( trait_exists('D', false) );
echo "\nCalling trait_exists() on existing trait with autoload explicitly disabled:\n";
var_dump( trait_exists('MyTrait', false) );

echo "\nCalling trait_exists() on non-existent trait with autoload unspecified:\n";
var_dump( trait_exists('E') );
echo "\nCalling trait_exists() on existing trait with autoload unspecified:\n";
var_dump( trait_exists('MyTrait') );

echo "Done";
?>
--EXPECT--
*** Testing trait_exists() : basic functionality ***
Calling trait_exists() on non-existent trait with autoload explicitly enabled:
In autoload(C)
bool(false)

Calling trait_exists() on existing trait with autoload explicitly enabled:
bool(true)

Calling trait_exists() on non-existent trait with autoload explicitly enabled:
bool(false)

Calling trait_exists() on existing trait with autoload explicitly disabled:
bool(true)

Calling trait_exists() on non-existent trait with autoload unspecified:
In autoload(E)
bool(false)

Calling trait_exists() on existing trait with autoload unspecified:
bool(true)
Done
