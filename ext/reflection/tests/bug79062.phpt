--TEST--
Bug #79062: Property with heredoc default value returns false for getDocComment
--FILE--
<?php

class BugReportMailrcConfigTests
{
    /** @var string */
    private $s1 = <<<STRING
I'm a string :(
STRING;
    /** @var string */
    private $s2 = <<<'STRING'
I'm a string :)
STRING;
    /** @var string */
    private $s3 = 'I\'m a string :)';
}

$ref = new \ReflectionClass(BugReportMailrcConfigTests::class);
$s1 = $ref->getProperty('s1');
var_dump($s1->getDocComment());

$s2 = $ref->getProperty('s2');
var_dump($s2->getDocComment());

$s3 = $ref->getProperty('s3');
var_dump($s2->getDocComment());

?>
--EXPECT--
string(18) "/** @var string */"
string(18) "/** @var string */"
string(18) "/** @var string */"
