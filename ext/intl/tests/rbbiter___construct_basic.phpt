--TEST--
IntlRuleBasedBreakIterator::__construct: basic test
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$rules = <<<RULES
\$LN = [[:letter:] [:number:]];
\$S = [.;,:];

!!forward;
\$LN+ {1};
\$S+ {42};
!!reverse;
\$LN+ {1};
\$S+ {42};
!!safe_forward;
!!safe_reverse;
RULES;
$rbbi = new IntlRuleBasedBreakIterator($rules);
var_dump(get_class($rbbi));
?>
==DONE==
--EXPECT--
string(26) "IntlRuleBasedBreakIterator"
==DONE==