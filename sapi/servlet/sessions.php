<html>
<body bgcolor="white">
<head>
<title>Sessions Example</title>
</head>
<body>
<a href="/examples/servlets/sessions.html">
<img src="/examples/images/code.gif" height=24 width=24 align=right border=0 alt="view code"></a>
<a href="/examples/servlets/index.html">
<img src="/examples/images/return.gif" height=24 width=24 align=right border=0 alt="return"></a>
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

  $dataName = $request->getParameter("dataName");
  if ($dataName) {
    $dataValue = $request->getParameter("dataValue");
    $dataValue = $request->getParameter("dataValue");
    $session->setAttribute($dataName, $dataValue);
  }

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
</body>
</html>
</body>
</html>
