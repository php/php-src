<html>
<body>
<head>
<title>Request Parameters Example</title>
</head>
<body bgcolor="white">
<h3>Request Parameters Example</h3>
Parameters in this request:<br>
<?php
  $e = $request->getParameterNames();

  if (!$e->hasMoreElements()) {
    echo "No Parameters, Please enter some"; 
  }

  while ($e->hasMoreElements()) {
    $name = $e->nextElement();
    $value = $request->getParameter($name);
    echo "$name = $value<br>\n";
  }

?>
<P>
<form action="<?php echo $PHP_SELF ?>" method=POST>
First Name:
<input type=text size=20 name=firstname>
<br>
Last Name:
<input type=text size=20 name=lastname>
<br>
<input type=submit>
</form>
</body>
</html>

