<html>
<body bgcolor="white">
<head>
<title>Request Header Example</title>
</head>
<body>
<a href="/examples/servlets/reqheaders.html">
<img src="/examples/images/code.gif" height=24 width=24 align=right border=0 alt="view code"></a>
<a href="/examples/servlets/index.html">
<img src="/examples/images/return.gif" height=24 width=24 align=right border=0 alt="return"></a>
<h3>Request Header Example</h3>
<table border=0>

<?php

   $e = $request->headerNames;
   while ($e->hasMoreElements()) {
     $name = $e->nextElement();
     $value = $request->getHeader($name);
     print "<tr><td bgcolor=\"#CCCCCC\">$name\n";
     print "</td><td>$value</td></tr>\n";
    }

?>

</table>
</body>
</html>
