--TEST--
IntlRuleBasedBreakIterator::__construct: basic test
--EXTENSIONS--
intl
--FILE--
<?php
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

try {
    $obj = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+', 'aoeu');
} catch (IntlException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(26) "IntlRuleBasedBreakIterator"
IntlRuleBasedBreakIterator::__construct(): unable to create instance from compiled rules
