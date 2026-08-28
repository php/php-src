--TEST--
IntlRuleBasedBreakIterator compiled rules outlive the source string
--EXTENSIONS--
intl
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

$src = new IntlRuleBasedBreakIterator($rules);
$len = strlen($src->getBinaryRules());

$it = new IntlRuleBasedBreakIterator($src->getBinaryRules(), true);
unset($src);

/* ICU aliases the buffer it was built from, so the freed rules have to be
   reclaimed and overwritten for the iterator below to read stale bytes. */
$ballast = [];
for ($i = 0; $i < 16; $i++) {
    $ballast[] = str_repeat("\xCC", $len);
}

$it->setText('ab,cd');
echo $it->first(), "\n";
while (true) {
    $n = $it->next();
    if ($n === IntlBreakIterator::DONE) {
        break;
    }
    echo $n, "\n";
}

$clone = clone $it;
unset($it);
$ballast[] = str_repeat("\xDD", $len);
$clone->setText('xy');
echo $clone->first(), "\n";
echo $clone->next(), "\n";

$src = new IntlRuleBasedBreakIterator($rules);
$it = new IntlRuleBasedBreakIterator($src->getBinaryRules(), true);
unset($src);
for ($i = 0; $i < 16; $i++) {
    $ballast[] = str_repeat("\xEE", $len);
}
$it->setText('ab,cd');
$parts = $it->getPartsIterator();
unset($it);
foreach ($parts as $p) {
    echo $p, "\n";
}

?>
--EXPECT--
0
2
3
5
0
2
ab
,
cd
