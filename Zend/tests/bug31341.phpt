--TEST--
Bug #31341 (escape on curly inconsistent)
--FILE--
<?php
$a = array(
    "$     \{    ",
    "      \{   $",
    "      \{$   ",
    "      $\{   ",
    "      \$\{  ",
    "      \{\$  ",
    "\$    \{    ",
    "      \{  \$",
    "%     \{    ");

foreach ($a as $v) {
    echo("'$v'\n");
}
?>
--EXPECT--
'$     \{    '
'      \{   $'
'      \{$   '
'      $\{   '
'      $\{  '
'      \{$  '
'$    \{    '
'      \{  $'
'%     \{    '
