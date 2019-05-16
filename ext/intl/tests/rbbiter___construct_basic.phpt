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

try {
	$obj = new IntlRuleBasedBreakIterator('[\p{Letter}\uFFFD]+;[:number:]+', 'aoeu');
} catch (IntlException $e) {
	var_dump(intl_get_error_code(), intl_get_error_message());
}

?>
==DONE==
--EXPECT--
string(26) "IntlRuleBasedBreakIterator"
int(1)
string(93) "rbbi_create_instance: unable to create instance from compiled rules: U_ILLEGAL_ARGUMENT_ERROR"
==DONE==
