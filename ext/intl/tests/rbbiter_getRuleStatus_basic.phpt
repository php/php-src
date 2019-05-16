--TEST--
IntlRuleBasedBreakIterator::getRuleStatus(): basic test
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
$rbbi->setText('sdfkjsdf88á.... ,;');

do {
	echo "pos        : {$rbbi->current()}\n",
		 "rule status: {$rbbi->getRuleStatus()}\n";
} while ($rbbi->next() != IntlBreakIterator::DONE);

?>
==DONE==
--EXPECT--
pos        : 0
rule status: 0
pos        : 12
rule status: 1
pos        : 16
rule status: 42
pos        : 17
rule status: 0
pos        : 19
rule status: 42
==DONE==
