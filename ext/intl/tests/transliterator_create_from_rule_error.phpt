--TEST--
Transliterator::createFromRules (error)
--EXTENSIONS--
intl
--FILE--
<?php

$t = Transliterator::createFromRules("\x8Fss");
echo intl_get_error_message(),"\n";

$rules = <<<RULES
\`\` } a > “;
\`\` } a > b;
RULES;

$t = Transliterator::createFromRules($rules);
echo intl_get_error_message(),"\n";

$rules = <<<RULES
ffff
RULES;

$t = Transliterator::createFromRules($rules);
echo intl_get_error_message(),"\n";
echo "Done.\n";
?>
--EXPECT--
Transliterator::createFromRules(): String conversion of rules to UTF-16 failed: U_INVALID_CHAR_FOUND
Transliterator::createFromRules(): unable to create ICU transliterator from rules (parse error after "{'``'}a > “;", before or at "{'``'}a > b;"): U_RULE_MASK_ERROR
Transliterator::createFromRules(): unable to create ICU transliterator from rules (parse error at offset 0, before or at "ffff"): U_MISSING_OPERATOR
Done.
