--TEST--
Levenshtein suggestion for undefined function calls
--FILE--
<?php
// One edit away — should suggest
try { strlenn("x"); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// One edit away for a longer name — should suggest
try { array_pussh([], 1); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Two edits away for a name >= 8 chars — adaptive threshold should suggest
try { arry_pussh([], 1); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Completely wrong name — no suggestion
try { nonexistentfunc(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Dynamic call — same suggestion logic applies
$f = "strlenx";
try { $f("x"); } catch (Error $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECT--
Call to undefined function strlenn() (did you mean strlen()?)
Call to undefined function array_pussh() (did you mean array_push()?)
Call to undefined function arry_pussh() (did you mean array_push()?)
Call to undefined function nonexistentfunc()
Call to undefined function strlenx() (did you mean strlen()?)
