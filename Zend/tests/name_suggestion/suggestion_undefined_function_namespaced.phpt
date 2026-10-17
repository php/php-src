--TEST--
Levenshtein suggestion for undefined function calls in a namespace
--FILE--
<?php
namespace App;

function helper() {}

// Unqualified call — falls back to a similar global function
try { strlenn("x"); } catch (\Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Unqualified call — similar function in the current namespace wins
try { helpr(); } catch (\Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Qualified call — only namespaced functions are considered
try { \App\helpr(); } catch (\Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { \App\strlenn("x"); } catch (\Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// Dynamic call with a namespaced name
$f = 'App\helpr';
try { $f(); } catch (\Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

// FCC in a default argument — falls back to a similar global function
function test(\Closure $c = strlenn(...)) {}
try { test(); } catch (\Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
?>
--EXPECT--
Error: Call to undefined function App\strlenn() (did you mean strlen?)
Error: Call to undefined function App\helpr() (did you mean App\helper?)
Error: Call to undefined function App\helpr() (did you mean App\helper?)
Error: Call to undefined function App\strlenn()
Error: Call to undefined function App\helpr() (did you mean App\helper?)
Error: Call to undefined function App\strlenn() (did you mean strlen?)
