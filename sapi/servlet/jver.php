<html>
<?

  $system = new Java("java.lang.System");
  print "Java version=".$system->getProperty("java.version")." <br>\n";
  print "Java vendor=".$system->getProperty("java.vendor")." <p>\n\n";
  print "OS=".$system->getProperty("os.name")." ".
	      $system->getProperty("os.version")." on ".
	      $system->getProperty("os.arch")." <br>\n";

  $formatter = new Java("java.text.SimpleDateFormat",
			"EEEE, MMMM dd, yyyy 'at' h:mm:ss a zzzz");

  print $formatter->format(new Java("java.util.Date"))."\n";

?>
</html>
