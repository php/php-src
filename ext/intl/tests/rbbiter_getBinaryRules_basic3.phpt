--TEST--
IntlRuleBasedBreakIterator::getBinaryRules(): basic test icu >= 68.1
--EXTENSIONS--
intl
--INI--
intl.default_locale=pt_PT
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '68.1') < 0) die('skip for ICU >= 68.1'); ?>
--FILE--
<?php

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

$br = $rbbi->getBinaryRules();

$rbbi2 = new IntlRuleBasedBreakIterator($br, true);

var_dump($rbbi->getRules(), $rbbi2->getRules());
var_dump($rbbi->getRules() == $rbbi2->getRules());
?>
==DONE==
--EXPECT--
string(119) "$LN=[[:letter:][:number:]];$S=[.;,:];!!forward;$LN+{1};$S+{42};!!reverse;$LN+{1};$S+{42};!!safe_forward;!!safe_reverse;"
string(119) "$LN=[[:letter:][:number:]];$S=[.;,:];!!forward;$LN+{1};$S+{42};!!reverse;$LN+{1};$S+{42};!!safe_forward;!!safe_reverse;"
bool(true)
==DONE==
