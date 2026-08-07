--TEST--
Levenshtein suggestion for undefined class, interface and trait lookups
--FILE--
<?php
// One edit away — should suggest
try { new ArrayObjekt(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { new StdClas(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// One edit away for a longer name — should suggest
try { new ArrayIteratr(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Two edits away for a name >= 8 chars — adaptive threshold should suggest
try { new ArryObjct(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Completely wrong name — no suggestion
try { new Unicorn(); } catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Interface
try {
    $c = new class() implements Countble {};
} catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Trait
try {
    eval('class T { use NonExistntTrait; }');
} catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
?>
--EXPECTF--
Error: Class "ArrayObjekt" not found (did you mean ArrayObject?)
Error: Class "StdClas" not found (did you mean stdClass?)
Error: Class "ArrayIteratr" not found (did you mean ArrayIterator?)
Error: Class "ArryObjct" not found (did you mean ArrayObject?)
Error: Class "Unicorn" not found
Error: Interface "Countble" not found (did you mean Countable?)
Error: Trait "NonExistntTrait" not found
