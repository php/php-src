--TEST--
IntlRuleBasedBreakIterator::getRuleStatusVec(): basic test
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
[^.]+ {4};
\$S+ {42};
!!reverse;
\$LN+ {1};
[^.]+ {4};
\$S+ {42};
!!safe_forward;
!!safe_reverse;
RULES;
$rbbi = new IntlRuleBasedBreakIterator($rules);
$rbbi->setText('sdfkjsdf88á.... ,;');

do {
    var_dump($rbbi->current(), $rbbi->getRuleStatusVec());
} while ($rbbi->next() != IntlBreakIterator::DONE);

?>
--EXPECT--
int(0)
array(1) {
  [0]=>
  int(0)
}
int(12)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(4)
}
int(16)
array(1) {
  [0]=>
  int(42)
}
int(19)
array(1) {
  [0]=>
  int(4)
}
