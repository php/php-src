<html>
<body bgcolor="white">
<head>
<title>Sessions Example</title>
</head>
<body>
<h3>Sessions Example</h3>
<?php

  // print session info 

  $session  = $request->session;
  $created  = new Java("java.util.Date", $session->creationTime);
  $accessed = new Java("java.util.Date", $session->lastAccessedTime);

  print "Session ID: $session->id<br>\n";
  print "Created: " . $created->toString() . "<br>\n";
  print "Last Accessed: " . $accessed->toString() . "<br>\n";

  // set session info if needed

  if ($dataName) $session->setAttribute($dataName, $dataValue);

  // print session contents

  print "<P>\n";
  print "The following data is in your session:<br>\n";
  $e = $session->attributeNames;
  while ($e->hasMoreElements()) {
    $name = $e->nextElement();
    $value = $session->getAttribute($name);
    print "$name = $value<br>\n";
  }

?>
<P>
<form action="<?php echo $PHP_SELF ?>" method=POST>
Name of Session Attribute:
<input type=text size=20 name=dataName>
<br>
Value of Session Attribute:
<input type=text size=20 name=dataValue>
<br>
<input type=submit>
</form>
<P>GET based form:<br>
<form action="<?php echo $PHP_SELF ?>" method=GET>
Name of Session Attribute:
<input type=text size=20 name=dataName>
<br>
Value of Session Attribute:
<input type=text size=20 name=dataValue>
<br>
<input type=submit>
</form>
<p><a href="<?php echo $PHP_SELF ?>?dataName=foo&dataValue=bar" >URL encoded </a>
</body>
</html>
</body>
</html>
