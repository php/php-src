--TEST--
tidyNode tests
--SKIPIF--
<?php if (!extension_loaded('tidy')) die('skip'); ?>
--FILE--
<?php

$tidy=tidy_parse_string('<% %>', array('newline' => 'LF'));
var_dump($tidy->Root()->child[0]->isAsp());

$tidy=tidy_parse_string('<# #>',  array('newline' => 'LF'));
var_dump($tidy->Root()->child[0]->isJste());

$tidy=tidy_parse_string('<html><body>text</body></html>');
var_dump($tidy->Root()->child[0]->child[1]->child[0]->isText());

$tidy=tidy_parse_string('<html><body><!-- comment --></body></html>', array('newline' => 'LF'));
$n = $tidy->Root()->child[0]->child[1]->child[0];
var_dump($n->isComment());
var_dump((string)$n);
var_dump((bool)$n);
var_dump((double)$n);
var_dump((int)$n);
var_dump($tidy->Root()->child[0]->child[0]->hasSiblings());

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
string(16) "<!-- comment -->"
bool(true)
float(0)
int(0)
bool(true)
