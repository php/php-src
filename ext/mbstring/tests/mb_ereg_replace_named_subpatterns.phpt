--TEST--
mb_ereg_replace() with named subpatterns
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
    mb_regex_set_options('');
    // \k<word> syntax
    echo mb_ereg_replace('(?<a>\s*)(?<b>\w+)(?<c>\s*)', '\k<a>_\k<b>_\k<c>', 'a b c d e' ), "\n";
    // \k'word' syntax
    echo mb_ereg_replace('(?<word>[a-z]+)',"<\k'word'>", 'abc def ghi'), PHP_EOL;
    // numbered captures with \k<n> syntax
    echo mb_ereg_replace('(1)(2)(3)(4)(5)(6)(7)(8)(9)(a)(\10)', '\k<0>-\k<10>-', '123456789aa'), PHP_EOL;
    // numbered captures with \k'n' syntax
    echo mb_ereg_replace('(1)(2)(3)(4)(5)(6)(7)(8)(9)(a)(\10)', "\k'0'-\k'10'-", '123456789aa'), PHP_EOL;
    // backref 0 works, but 01 is ignored
    echo mb_ereg_replace('a', "\k'0'_\k<01>", 'a'), PHP_EOL;
    // Numbered backref is ignored if named backrefs are present
    echo mb_ereg_replace('(?<a>A)\k<a>', '-\1-', 'AA'), PHP_EOL;
    // An empty backref is ignored
    echo mb_ereg_replace('(\w)\1', '-\k<>-', 'AA'), PHP_EOL;
    // An unclosed backref is ignored
    echo mb_ereg_replace('(?<a>\w+)', '-\k<a', 'AA'), PHP_EOL;
?>
--EXPECT--
_a_ _b_ _c_ _d_ _e_
<abc> <def> <ghi>
123456789aa-a-
123456789aa-a-
a_\k<01>
-\1-
-\k<>-
-\k<a
