<html>
<body>
<head>
<title>Request Parameters Example</title>
</head>
<body bgcolor="white">
<a href="/examples/servlets/reqparams.html">
<img src="/examples/images/code.gif" height=24 width=24 align=right border=0 alt="view code"></a>
<a href="/examples/servlets/index.html">
<img src="/examples/images/return.gif" height=24 width=24 align=right border=0 alt="return"></a>
<h3>Request Parameters Example</h3>
Parameters in this request:<br>
<?php
  $e = $request->parameterNames;

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

