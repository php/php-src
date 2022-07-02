--TEST--
ReflectionMethod::getDocComment() uses left over doc comment from previous scanner run
--EXTENSIONS--
tokenizer
--INI--
opcache.save_comments=1
--FILE--
<?php

function strip_doc_comment($c)
{
    if (!strlen($c) || $c === false) return $c;
    return trim(substr($c, 3, -2));
}

token_get_all("<?php\n/**\n * Foo\n */"); // doc_comment compiler global now contains this Foo comment

eval('class A { }'); // Could also be an include of a file containing similar

$ra = new ReflectionClass('A');
var_dump(strip_doc_comment($ra->getDocComment()));

token_get_all("<?php\n/**\n * Foo\n */"); // doc_comment compiler global now contains this Foo comment

include('bug64936.inc');

$rb = new ReflectionClass('B');
var_dump(strip_doc_comment($rb->getDocComment()));

?>
--EXPECT--
bool(false)
bool(false)
