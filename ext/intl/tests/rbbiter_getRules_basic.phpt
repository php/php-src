--TEST--
IntlRuleBasedBreakIterator::getRules(): basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=pt_PT
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '61.1') >=  0) die('skip for ICU < 61.1'); ?>
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
var_dump($rbbi->getRules());

?>
--EXPECT--
string(128) "$LN = [[:letter:] [:number:]];$S = [.;,:];!!forward;$LN+ {1};$S+ {42};!!reverse;$LN+ {1};$S+ {42};!!safe_forward;!!safe_reverse;"
