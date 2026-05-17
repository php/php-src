--TEST--
Levenshtein suggestion for undefined method calls
--FILE--
<?php
class Calculator {
    public function add(int $a, int $b): int { return $a + $b; }
    public function subtract(int $a, int $b): int { return $a - $b; }
    public function multiply(int $a, int $b): int { return $a * $b; }
}

$calc = new Calculator();

// One edit away — should suggest
try { $calc->addd(1, 2); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Two edits away for a name >= 8 chars — adaptive threshold should suggest
try { $calc->subtarct(5, 3); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Completely wrong name — no suggestion
try { $calc->nonexistent(); } catch (Error $e) { echo $e->getMessage(), "\n"; }

// Static call — same logic applies
try { Calculator::addd(1, 2); } catch (Error $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECT--
Call to undefined method Calculator::addd() (did you mean add()?)
Call to undefined method Calculator::subtarct() (did you mean subtract()?)
Call to undefined method Calculator::nonexistent()
Call to undefined method Calculator::addd() (did you mean add()?)
