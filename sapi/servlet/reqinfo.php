<html>
<body>
<head>
<title>Request Information Example</title>
</head>
<body bgcolor="white">
<a href="/examples/servlets/reqinfo.html">
<img src="/examples/images/code.gif" height=24 width=24 align=right border=0 alt="view code"></a>
<a href="/examples/servlets/index.html">
<img src="/examples/images/return.gif" height=24 width=24 align=right border=0 alt="return"></a>
<h3>Request Information Example</h3>
<table border=0><tr><td>
Method:
</td><td>
<?php print $request->method ?>
</td></tr><tr><td>
Request URI:
</td><td>
<?php print $request->requestURI ?>
</td></tr><tr><td>
Protocol:
</td><td>
<?php print $request->protocol ?>
</td></tr><tr><td>
Path Info:
</td><td>
<?php print $request->pathInfo ?>
</td></tr><tr><td>
Remote Address:
</td><td>
<?php print $request->remoteAddr ?>
</table>

