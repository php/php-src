--TEST--
short_open_tag: On, asp_tags: On
--INI--
short_open_tag=on
asp_tags=on
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
--EXPECT--
this should get echoed
so should this

This gets echoed twice
This gets echoed twice

3

