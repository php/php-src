--TEST--
IntlRuleBasedBreakIterator::getRuleStatus(): basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=pt_PT
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

do {
    echo "pos        : {$rbbi->current()}\n",
         "rule status: {$rbbi->getRuleStatus()}\n";
} while ($rbbi->next() != IntlBreakIterator::DONE);

?>
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
