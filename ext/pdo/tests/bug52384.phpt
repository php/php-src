--TEST--
PDOStatement::debugDumpParams() - basic test.
--FILE--
<?php

$dbh = new PDO("sqlite::memory:");
$calories = 150;
$colour = 'red';
$tf = true;
$n = null;
$lob = str_repeat('a',1000);

$sth = $dbh->prepare('SELECT 1 WHERE 1');
$sth->bindParam(':calories', $calories, PDO::PARAM_INT);
$sth->bindValue(':colour', $colour, PDO::PARAM_STR);
$sth->bindValue(':tf', $tf, PDO::PARAM_BOOL);
$sth->bindValue(':n', $n, PDO::PARAM_NULL);
$sth->bindValue(':lob', $lob, PDO::PARAM_LOB);

$sth->debugDumpParams();
unset($dbh);
?>
--EXPECT--
SQL: [16] SELECT 1 WHERE 1
Params:  5
Key: Name: [9] :calories
Number=-1
Name=[9] ":calories"
Is Param=1
Type=PDO::PARAM_INT
Value=150

Key: Name: [7] :colour
Number=-1
Name=[7] ":colour"
Is Param=1
Type=PDO::PARAM_STR
Value=red

Key: Name: [3] :tf
Number=-1
Name=[3] ":tf"
Is Param=1
Type=PDO::PARAM_BOOL
Value=true

Key: Name: [2] :n
Number=-1
Name=[2] ":n"
Is Param=1
Type=PDO::PARAM_NULL
Value=null

Key: Name: [4] :lob
Number=-1
Name=[4] ":lob"
Is Param=1
Type=PDO::PARAM_LOB
Value=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

--CREDITS--
Chad Scribner <cwscribner@gmail.com>
