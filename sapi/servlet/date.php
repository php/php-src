<html>
<!--
  Copyright (c) 1999 The Apache Software Foundation.  All rights 
  reserved.
-->

<body bgcolor="white">
<?php $clock=new Java("dates.JspCalendar"); ?>

<font size=4>
<ul>
<li>	Day of month: is  <?php echo $clock->dayOfMonth ?>
<li>	Year: is  <?php echo $clock->year ?>
<li>	Month: is  <?php echo $clock->month ?>
<li>	Time: is  <?php echo $clock->time ?>
<li>	Date: is  <?php echo $clock->date ?>
<li>	Day: is  <?php echo $clock->day ?>
<li>	Day Of Year: is  <?php echo $clock->dayOfYear ?>
<li>	Week Of Year: is  <?php echo $clock->weekOfYear ?>
<li>	era: is  <?php echo $clock->era ?>
<li>	DST Offset: is  <?php echo $clock->dSTOffset ?>
<li>	Zone Offset: is  <?php echo $clock->zoneOffset ?>
</ul>
</font>

</body>
</html>

