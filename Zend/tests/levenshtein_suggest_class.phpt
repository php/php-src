--TEST--
Levenshtein suggestion for undefined class, interface and trait lookups
--FILE--
<?php
// One edit away — should suggest
try { new ArrayObjekt(); } catch (Error $e) { echo $e->getMessage(), "\n"; }
try { new StdClas(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// One edit away for a longer name — should suggest
try { new ArrayIteratr(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Two edits away for a name >= 8 chars — adaptive threshold should suggest
try { new ArryObjct(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Completely wrong name — no suggestion
try { new Unicorn(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Interface
try {
    $c = new class() implements Countble {};
} catch (Error $e) { echo $e->getMessage(), "\n"; }

// Trait
try {
    eval('class T { use NonExistntTrait; }');
} catch (Error $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECTF--
Class "ArrayObjekt" not found (did you mean ArrayObject?)
Class "StdClas" not found (did you mean stdClass?)
Class "ArrayIteratr" not found (did you mean ArrayIterator?)
Class "ArryObjct" not found (did you mean ArrayObject?)
Class "Unicorn" not found
Interface "Countble" not found (did you mean Countable?)
Trait "NonExistntTrait" not found
