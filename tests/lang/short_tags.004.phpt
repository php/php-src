--TEST--
tags
--INI--
short_open_tag=off
asp_tags=off
--FILE--
<?='this should get echoed'?>

<%= 'so should this' %>

<?php
$a = 'This gets echoed twice';
?>

<?= $a?>

<%= $a%>

<? $b=3; ?>

<?php
   echo "{$b}";
?>
--EXPECTF--
<?='this should get echoed'?>

<%= 'so should this' %>


<?= $a?>

<%= $a%>

<? $b=3; ?>


Notice: Undefined variable: b in %s on line %d