--TEST--
short_open_tag: Off, asp_tags: Off
--INI--
short_open_tag=off
asp_tags=off
--FILE--
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
<?= "{$b}"?>
--EXPECTF--
<%= 'so should this' %>


This gets echoed twice
<%= $a%>

<? $b=3; ?>


Notice: Undefined variable: b in %s on line %d

Notice: Undefined variable: b in %s on line %d
